///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015-2020 Edouard Griffiths, F4EXB                              //
//                                                                               //
// Symbol synchronizer or symbol clock recovery mostly encapsulating             //
// liquid-dsp's symsync "object"                                                 //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
// (at your option) any later version.                                           //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#include "SWGGLSpectrum.h"
#include "SWGSpectrumServer.h"
#include "SWGSuccessResponse.h"

#include "glspectruminterface.h"
#include "dspcommands.h"
#include "dspengine.h"
#include "fftfactory.h"
#include "util/messagequeue.h"

#include "spectrumvis.h"

#define MAX_FFT_SIZE 4096

#ifndef LINUX
inline double log2f(double n)
{
	return log(n) / log(2.0);
}
#endif

MESSAGE_CLASS_DEFINITION(SpectrumVis::MsgConfigureSpectrumVis, Message)
MESSAGE_CLASS_DEFINITION(SpectrumVis::MsgConfigureScalingFactor, Message)
MESSAGE_CLASS_DEFINITION(SpectrumVis::MsgConfigureWSpectrumOpenClose, Message)
MESSAGE_CLASS_DEFINITION(SpectrumVis::MsgConfigureWSpectrum, Message)
MESSAGE_CLASS_DEFINITION(SpectrumVis::MsgStartStop, Message)

const Real SpectrumVis::m_mult = (10.0f / log2f(10.0f));

SpectrumVis::SpectrumVis(Real scalef) :
	BasebandSampleSink(),
    m_running(true),
	m_fft(nullptr),
    m_fftEngineSequence(0),
	m_fftBuffer(MAX_FFT_SIZE),
	m_powerSpectrum(MAX_FFT_SIZE),
	m_fftBufferFill(0),
	m_needMoreSamples(false),
	m_scalef(scalef),
	m_glSpectrum(nullptr),
    m_centerFrequency(0),
    m_sampleRate(48000),
	m_ofs(0),
    m_powFFTDiv(1.0),
	m_mutex(QMutex::Recursive)
{
	setObjectName("SpectrumVis");
    applySettings(m_settings, true);
}

SpectrumVis::~SpectrumVis()
{
    FFTFactory *fftFactory = DSPEngine::instance()->getFFTFactory();
    fftFactory->releaseEngine(m_settings.m_fftSize, false, m_fftEngineSequence);
}

void SpectrumVis::setScalef(Real scalef)
{
    MsgConfigureScalingFactor* cmd = new MsgConfigureScalingFactor(scalef);
    getInputMessageQueue()->push(cmd);
}

void SpectrumVis::configureWSSpectrum(const QString& address, uint16_t port)
{
    MsgConfigureWSpectrum* cmd = new MsgConfigureWSpectrum(address, port);
    getInputMessageQueue()->push(cmd);
}

void SpectrumVis::feedTriggered(const SampleVector::const_iterator& triggerPoint, const SampleVector::const_iterator& end, bool positiveOnly)
{
	feed(triggerPoint, end, positiveOnly); // normal feed from trigger point
	/*
	if (triggerPoint == end)
	{
		// the following piece of code allows to terminate the FFT that ends past the end of scope captured data
		// that is the spectrum will include the captured data
		// just do nothing if you want the spectrum to be included inside the scope captured data
		// that is to drop the FFT that dangles past the end of captured data
		if (m_needMoreSamples) {
			feed(begin, end, positiveOnly);
			m_needMoreSamples = false;      // force finish
		}
	}
	else
	{
		feed(triggerPoint, end, positiveOnly); // normal feed from trigger point
	}*/
}

void SpectrumVis::feed(const Complex *begin, unsigned int length)
{
	if (!m_glSpectrum && !m_wsSpectrum.socketOpened()) {
		return;
	}

    if (!m_mutex.tryLock(0)) { // prevent conflicts with configuration process
        return;
    }

    Complex c;
    Real v;

    if (m_settings.m_averagingMode == GLSpectrumSettings::AvgModeNone)
    {
        for (unsigned int i = 0; i < m_settings.m_fftSize; i++)
        {
            if (i < length) {
                c = begin[i];
            } else {
                c = Complex{0,0};
            }

            v = c.real() * c.real() + c.imag() * c.imag();
            v = m_settings.m_linear ? v/m_powFFTDiv : m_mult * log2f(v) + m_ofs;
            m_powerSpectrum[i] = v;
        }

        // send new data to visualisation
        if (m_glSpectrum) {
            m_glSpectrum->newSpectrum(m_powerSpectrum, m_settings.m_fftSize);
        }

        // web socket spectrum connections
        if (m_wsSpectrum.socketOpened())
        {
            m_wsSpectrum.newSpectrum(
                m_powerSpectrum,
                m_settings.m_fftSize,
                m_centerFrequency,
                m_sampleRate,
                m_settings.m_linear,
                m_settings.m_ssb,
                m_settings.m_usb
            );
        }
    }
    else if (m_settings.m_averagingMode == GLSpectrumSettings::AvgModeMoving)
    {
        for (unsigned int i = 0; i < m_settings.m_fftSize; i++)
        {
            if (i < length) {
                c = begin[i];
            } else {
                c = Complex{0,0};
            }

            v = c.real() * c.real() + c.imag() * c.imag();
            v = m_movingAverage.storeAndGetAvg(v, i);
            v = m_settings.m_linear ? v/m_powFFTDiv : m_mult * log2f(v) + m_ofs;
            m_powerSpectrum[i] = v;
        }

        // send new data to visualisation
        if (m_glSpectrum) {
            m_glSpectrum->newSpectrum(m_powerSpectrum, m_settings.m_fftSize);
        }

        // web socket spectrum connections
        if (m_wsSpectrum.socketOpened())
        {
            m_wsSpectrum.newSpectrum(
                m_powerSpectrum,
                m_settings.m_fftSize,
                m_centerFrequency,
                m_sampleRate,
                m_settings.m_linear,
                m_settings.m_ssb,
                m_settings.m_usb
            );
        }

        m_movingAverage.nextAverage();
    }
    else if (m_settings.m_averagingMode == GLSpectrumSettings::AvgModeFixed)
    {
        double avg;

        for (unsigned int i = 0; i < m_settings.m_fftSize; i++)
        {
            if (i < length) {
                c = begin[i];
            } else {
                c = Complex{0,0};
            }

            v = c.real() * c.real() + c.imag() * c.imag();

            // result available
            if (m_fixedAverage.storeAndGetAvg(avg, v, i))
            {
                avg = m_settings.m_linear ? avg/m_powFFTDiv : m_mult * log2f(avg) + m_ofs;
                m_powerSpectrum[i] = avg;
            }
        }

        // result available
        if (m_fixedAverage.nextAverage())
        {
            // send new data to visualisation
            if (m_glSpectrum) {
                m_glSpectrum->newSpectrum(m_powerSpectrum, m_settings.m_fftSize);
            }

            // web socket spectrum connections
            if (m_wsSpectrum.socketOpened())
            {
                m_wsSpectrum.newSpectrum(
                    m_powerSpectrum,
                    m_settings.m_fftSize,
                    m_centerFrequency,
                    m_sampleRate,
                    m_settings.m_linear,
                    m_settings.m_ssb,
                    m_settings.m_usb
                );
            }
        }
    }
    else if (m_settings.m_averagingMode == GLSpectrumSettings::AvgModeMax)
    {
        double max;

        for (unsigned int i = 0; i < m_settings.m_fftSize; i++)
        {
            if (i < length) {
                c = begin[i];
            } else {
                c = Complex{0,0};
            }

            v = c.real() * c.real() + c.imag() * c.imag();

            // result available
            if (m_max.storeAndGetMax(max, v, i))
            {
                max = m_settings.m_linear ? max/m_powFFTDiv : m_mult * log2f(max) + m_ofs;
                m_powerSpectrum[i] = max;
            }
        }

        // result available
        if (m_max.nextMax())
        {
            // send new data to visualisation
            if (m_glSpectrum) {
                m_glSpectrum->newSpectrum(m_powerSpectrum, m_settings.m_fftSize);
            }

            // web socket spectrum connections
            if (m_wsSpectrum.socketOpened())
            {
                m_wsSpectrum.newSpectrum(
                    m_powerSpectrum,
                    m_settings.m_fftSize,
                    m_centerFrequency,
                    m_sampleRate,
                    m_settings.m_linear,
                    m_settings.m_ssb,
                    m_settings.m_usb
                );
            }
        }
    }

    m_mutex.unlock();
}

void SpectrumVis::feed(const SampleVector::const_iterator& cbegin, const SampleVector::const_iterator& end, bool positiveOnly)
{
    if (!m_running) {
        return;
    }

	// if no visualisation is set, send the samples to /dev/null
	if (!m_glSpectrum && !m_wsSpectrum.socketOpened()) {
		return;
	}

    if (!m_mutex.tryLock(0)) { // prevent conflicts with configuration process
        return;
    }

	SampleVector::const_iterator begin(cbegin);

	while (begin < end)
	{
		std::size_t todo = end - begin;
		std::size_t samplesNeeded = m_refillSize - m_fftBufferFill;

		if (todo >= samplesNeeded)
		{
			// fill up the buffer
			std::vector<Complex>::iterator it = m_fftBuffer.begin() + m_fftBufferFill;

			for (std::size_t i = 0; i < samplesNeeded; ++i, ++begin)
			{
				*it++ = Complex(begin->real() / m_scalef, begin->imag() / m_scalef);
			}

			// apply fft window (and copy from m_fftBuffer to m_fftIn)
			m_window.apply(&m_fftBuffer[0], m_fft->in());

			// calculate FFT
			m_fft->transform();

			// extract power spectrum and reorder buckets
			const Complex* fftOut = m_fft->out();
			Complex c;
			Real v;
			std::size_t halfSize = m_settings.m_fftSize / 2;

			if (m_settings.m_averagingMode == GLSpectrumSettings::AvgModeNone)
			{
                if ( positiveOnly )
                {
                    for (std::size_t i = 0; i < halfSize; i++)
                    {
                        c = fftOut[i];
                        v = c.real() * c.real() + c.imag() * c.imag();
                        v = m_settings.m_linear ? v/m_powFFTDiv : m_mult * log2f(v) + m_ofs;
                        m_powerSpectrum[i * 2] = v;
                        m_powerSpectrum[i * 2 + 1] = v;
                    }
                }
                else
                {
                    for (std::size_t i = 0; i < halfSize; i++)
                    {
                        c = fftOut[i + halfSize];
                        v = c.real() * c.real() + c.imag() * c.imag();
                        v = m_settings.m_linear ? v/m_powFFTDiv : m_mult * log2f(v) + m_ofs;
                        m_powerSpectrum[i] = v;

                        c = fftOut[i];
                        v = c.real() * c.real() + c.imag() * c.imag();
                        v = m_settings.m_linear ? v/m_powFFTDiv : m_mult * log2f(v) + m_ofs;
                        m_powerSpectrum[i + halfSize] = v;
                    }
                }

                // send new data to visualisation
                if (m_glSpectrum) {
                    m_glSpectrum->newSpectrum(m_powerSpectrum, m_settings.m_fftSize);
                }

                // web socket spectrum connections
                if (m_wsSpectrum.socketOpened())
                {
                    m_wsSpectrum.newSpectrum(
                        m_powerSpectrum,
                        m_settings.m_fftSize,
                        m_centerFrequency,
                        m_sampleRate,
                        m_settings.m_linear,
                        m_settings.m_ssb,
                        m_settings.m_usb
                    );
                }
			}
			else if (m_settings.m_averagingMode == GLSpectrumSettings::AvgModeMoving)
			{
	            if ( positiveOnly )
	            {
	                for (std::size_t i = 0; i < halfSize; i++)
	                {
	                    c = fftOut[i];
	                    v = c.real() * c.real() + c.imag() * c.imag();
	                    v = m_movingAverage.storeAndGetAvg(v, i);
	                    v = m_settings.m_linear ? v/m_powFFTDiv : m_mult * log2f(v) + m_ofs;
	                    m_powerSpectrum[i * 2] = v;
	                    m_powerSpectrum[i * 2 + 1] = v;
	                }
	            }
	            else
	            {
	                for (std::size_t i = 0; i < halfSize; i++)
	                {
	                    c = fftOut[i + halfSize];
	                    v = c.real() * c.real() + c.imag() * c.imag();
	                    v = m_movingAverage.storeAndGetAvg(v, i+halfSize);
	                    v = m_settings.m_linear ? v/m_powFFTDiv : m_mult * log2f(v) + m_ofs;
	                    m_powerSpectrum[i] = v;

	                    c = fftOut[i];
	                    v = c.real() * c.real() + c.imag() * c.imag();
	                    v = m_movingAverage.storeAndGetAvg(v, i);
	                    v = m_settings.m_linear ? v/m_powFFTDiv : m_mult * log2f(v) + m_ofs;
	                    m_powerSpectrum[i + halfSize] = v;
	                }
	            }

	            // send new data to visualisation
                if (m_glSpectrum) {
    	            m_glSpectrum->newSpectrum(m_powerSpectrum, m_settings.m_fftSize);
                }

                // web socket spectrum connections
                if (m_wsSpectrum.socketOpened())
                {
                    m_wsSpectrum.newSpectrum(
                        m_powerSpectrum,
                        m_settings.m_fftSize,
                        m_centerFrequency,
                        m_sampleRate,
                        m_settings.m_linear,
                        m_settings.m_ssb,
                        m_settings.m_usb
                    );
                }

	            m_movingAverage.nextAverage();
			}
			else if (m_settings.m_averagingMode == GLSpectrumSettings::AvgModeFixed)
			{
			    double avg;

                if ( positiveOnly )
                {
                    for (std::size_t i = 0; i < halfSize; i++)
                    {
                        c = fftOut[i];
                        v = c.real() * c.real() + c.imag() * c.imag();

                        if (m_fixedAverage.storeAndGetAvg(avg, v, i))
                        {
                            avg = m_settings.m_linear ? avg/m_powFFTDiv : m_mult * log2f(avg) + m_ofs;
                            m_powerSpectrum[i * 2] = avg;
                            m_powerSpectrum[i * 2 + 1] = avg;
                        }
                    }
                }
                else
                {
                    for (std::size_t i = 0; i < halfSize; i++)
                    {
                        c = fftOut[i + halfSize];
                        v = c.real() * c.real() + c.imag() * c.imag();

                        // result available
                        if (m_fixedAverage.storeAndGetAvg(avg, v, i+halfSize))
                        {
                            avg = m_settings.m_linear ? avg/m_powFFTDiv : m_mult * log2f(avg) + m_ofs;
                            m_powerSpectrum[i] = avg;
                        }

                        c = fftOut[i];
                        v = c.real() * c.real() + c.imag() * c.imag();

                        // result available
                        if (m_fixedAverage.storeAndGetAvg(avg, v, i))
                        {
                            avg = m_settings.m_linear ? avg/m_powFFTDiv : m_mult * log2f(avg) + m_ofs;
                            m_powerSpectrum[i + halfSize] = avg;
                        }
                    }
                }

                // result available
                if (m_fixedAverage.nextAverage())
                {
                    // send new data to visualisation
                    if (m_glSpectrum) {
                        m_glSpectrum->newSpectrum(m_powerSpectrum, m_settings.m_fftSize);
                    }

                    // web socket spectrum connections
                    if (m_wsSpectrum.socketOpened())
                    {
                        m_wsSpectrum.newSpectrum(
                            m_powerSpectrum,
                            m_settings.m_fftSize,
                            m_centerFrequency,
                            m_sampleRate,
                            m_settings.m_linear,
                            m_settings.m_ssb,
                            m_settings.m_usb
                        );
                    }
                }
			}
			else if (m_settings.m_averagingMode == GLSpectrumSettings::AvgModeMax)
			{
			    double max;

                if ( positiveOnly )
                {
                    for (std::size_t i = 0; i < halfSize; i++)
                    {
                        c = fftOut[i];
                        v = c.real() * c.real() + c.imag() * c.imag();

                        if (m_max.storeAndGetMax(max, v, i))
                        {
                            max = m_settings.m_linear ? max/m_powFFTDiv : m_mult * log2f(max) + m_ofs;
                            m_powerSpectrum[i * 2] = max;
                            m_powerSpectrum[i * 2 + 1] = max;
                        }
                    }
                }
                else
                {
                    for (std::size_t i = 0; i < halfSize; i++)
                    {
                        c = fftOut[i + halfSize];
                        v = c.real() * c.real() + c.imag() * c.imag();

                        // result available
                        if (m_max.storeAndGetMax(max, v, i+halfSize))
                        {
                            max = m_settings.m_linear ? max/m_powFFTDiv : m_mult * log2f(max) + m_ofs;
                            m_powerSpectrum[i] = max;
                        }

                        c = fftOut[i];
                        v = c.real() * c.real() + c.imag() * c.imag();

                        // result available
                        if (m_max.storeAndGetMax(max, v, i))
                        {
                            max = m_settings.m_linear ? max/m_powFFTDiv : m_mult * log2f(max) + m_ofs;
                            m_powerSpectrum[i + halfSize] = max;
                        }
                    }
                }

                // result available
                if (m_max.nextMax())
                {
                    // send new data to visualisation
                    if (m_glSpectrum) {
                        m_glSpectrum->newSpectrum(m_powerSpectrum, m_settings.m_fftSize);
                    }

                    // web socket spectrum connections
                    if (m_wsSpectrum.socketOpened())
                    {
                        m_wsSpectrum.newSpectrum(
                            m_powerSpectrum,
                            m_settings.m_fftSize,
                            m_centerFrequency,
                            m_sampleRate,
                            m_settings.m_linear,
                            m_settings.m_ssb,
                            m_settings.m_usb
                        );
                    }
                }
			}

			// advance buffer respecting the fft overlap factor
			std::copy(m_fftBuffer.begin() + m_refillSize, m_fftBuffer.end(), m_fftBuffer.begin());

			// start over
			m_fftBufferFill = m_overlapSize;
			m_needMoreSamples = false;
		}
		else
		{
			// not enough samples for FFT - just fill in new data and return
			for(std::vector<Complex>::iterator it = m_fftBuffer.begin() + m_fftBufferFill; begin < end; ++begin)
			{
				*it++ = Complex(begin->real() / m_scalef, begin->imag() / m_scalef);
			}

			m_fftBufferFill += todo;
			m_needMoreSamples = true;
		}
	}

	 m_mutex.unlock();
}

void SpectrumVis::start()
{
    m_running = true;
}

void SpectrumVis::stop()
{
    m_running = false;
}

bool SpectrumVis::handleMessage(const Message& message)
{
    if (DSPSignalNotification::match(message))
    {
        // This is coming from device engine and will apply to main spectrum
        DSPSignalNotification& notif = (DSPSignalNotification&) message;
        qDebug() << "SpectrumVis::handleMessage: DSPSignalNotification:"
            << " centerFrequency: " << notif.getCenterFrequency()
            << " sampleRate: " << notif.getSampleRate();
        handleConfigureDSP(notif.getCenterFrequency(), notif.getSampleRate());
        return true;
    }
	else if (MsgConfigureSpectrumVis::match(message))
	{
        MsgConfigureSpectrumVis& cfg = (MsgConfigureSpectrumVis&) message;
        qDebug() << "SpectrumVis::handleMessage: MsgConfigureSpectrumVis";
        applySettings(cfg.getSettings(), cfg.getForce());
		return true;
	}
    else if (MsgConfigureScalingFactor::match(message))
    {
        MsgConfigureScalingFactor& conf = (MsgConfigureScalingFactor&) message;
        handleScalef(conf.getScalef());
        return true;
    }
    else if (MsgConfigureWSpectrumOpenClose::match(message))
    {
        MsgConfigureWSpectrumOpenClose& conf = (MsgConfigureWSpectrumOpenClose&) message;
        handleWSOpenClose(conf.getOpenClose());
        return true;
    }
    else if (MsgConfigureWSpectrum::match(message)) {
        MsgConfigureWSpectrum& conf = (MsgConfigureWSpectrum&) message;
        handleConfigureWSSpectrum(conf.getAddress(), conf.getPort());
        return true;
    }
    else if (MsgStartStop::match(message)) {
        MsgStartStop& cmd = (MsgStartStop&) message;

        if (cmd.getStartStop()) {
            start();
        } else {
            stop();
        }

        return true;
    }
	else
	{
		return false;
	}
}

void SpectrumVis::applySettings(const GLSpectrumSettings& settings, bool force)
{
    QMutexLocker mutexLocker(&m_mutex);

    int fftSize = settings.m_fftSize > MAX_FFT_SIZE ?
        MAX_FFT_SIZE :
        settings.m_fftSize < 64 ?
            64 :
            settings.m_fftSize;

    int overlapPercent = settings.m_fftOverlap > 100 ?
        100 :
        settings.m_fftOverlap < 0 ?
            0 :
            settings.m_fftOverlap;

    qDebug() << "SpectrumVis::applySettings:"
        << " m_fftSize: " << fftSize
        << " m_fftWindow: " << settings.m_fftWindow
        << " m_fftOverlap: " << overlapPercent
        << " m_averagingIndex: " << settings.m_averagingIndex
        << " m_averagingMode: " << settings.m_averagingMode
        << " m_refLevel: " << settings.m_refLevel
        << " m_powerRange: " << settings.m_powerRange
        << " m_linear: " << settings.m_linear
        << " m_ssb: " << settings.m_ssb
        << " m_usb: " << settings.m_usb
        << " m_wsSpectrumAddress: " << settings.m_wsSpectrumAddress
        << " m_wsSpectrumPort: " << settings.m_wsSpectrumPort
        << " force: " << force;

    if ((fftSize != m_settings.m_fftSize) || force)
    {
        FFTFactory *fftFactory = DSPEngine::instance()->getFFTFactory();

        // release previous engine allocation if any
        if (m_fft) {
            fftFactory->releaseEngine(m_settings.m_fftSize, false, m_fftEngineSequence);
        }

        m_fftEngineSequence = fftFactory->getEngine(fftSize, false, &m_fft);
        m_ofs = 20.0f * log10f(1.0f / fftSize);
        m_powFFTDiv = fftSize * fftSize;
    }

    if ((fftSize != m_settings.m_fftSize)
     || (settings.m_fftWindow != m_settings.m_fftWindow) || force)
    {
        m_window.create(settings.m_fftWindow, fftSize);
    }

    if ((fftSize != m_settings.m_fftSize)
     || (overlapPercent != m_settings.m_fftOverlap) || force)
    {
        m_overlapSize = (fftSize * overlapPercent) / 100;
        m_refillSize = fftSize - m_overlapSize;
        m_fftBufferFill = m_overlapSize;
    }

    if ((fftSize != m_settings.m_fftSize)
     || (settings.m_averagingIndex != m_settings.m_averagingIndex)
     || (settings.m_averagingMode != m_settings.m_averagingMode) || force)
    {
        unsigned int averagingValue = GLSpectrumSettings::getAveragingValue(settings.m_averagingIndex, settings.m_averagingMode);
        m_movingAverage.resize(fftSize, averagingValue > 1000 ? 1000 : averagingValue); // Capping to avoid out of memory condition
        m_fixedAverage.resize(fftSize, averagingValue);
        m_max.resize(fftSize, averagingValue);
    }

    if ((settings.m_wsSpectrumAddress != m_settings.m_wsSpectrumAddress)
     || (settings.m_wsSpectrumPort != m_settings.m_wsSpectrumPort) || force) {
         handleConfigureWSSpectrum(settings.m_wsSpectrumAddress, settings.m_wsSpectrumPort);
    }

    m_settings = settings;
    m_settings.m_fftSize = fftSize;
    m_settings.m_fftOverlap = overlapPercent;
}

void SpectrumVis::handleConfigureDSP(uint64_t centerFrequency, int sampleRate)
{
    QMutexLocker mutexLocker(&m_mutex);
    m_centerFrequency = centerFrequency;
    m_sampleRate = sampleRate;
}

void SpectrumVis::handleScalef(Real scalef)
{
    QMutexLocker mutexLocker(&m_mutex);
    m_scalef = scalef;
}

void SpectrumVis::handleWSOpenClose(bool openClose)
{
    QMutexLocker mutexLocker(&m_mutex);

    if (openClose) {
        m_wsSpectrum.openSocket();
    } else {
        m_wsSpectrum.closeSocket();
    }
}

void SpectrumVis::handleConfigureWSSpectrum(const QString& address, uint16_t port)
{
    m_wsSpectrum.setListeningAddress(address);
    m_wsSpectrum.setPort(port);

    if (m_wsSpectrum.socketOpened())
    {
        m_wsSpectrum.closeSocket();
        m_wsSpectrum.openSocket();
    }
}

int SpectrumVis::webapiSpectrumSettingsGet(SWGSDRangel::SWGGLSpectrum& response, QString& errorMessage) const
{
    (void) errorMessage;
    response.init();
    webapiFormatSpectrumSettings(response, m_settings);
    return 200;
}

int SpectrumVis::webapiSpectrumSettingsPutPatch(
    bool force,
    const QStringList& spectrumSettingsKeys,
    SWGSDRangel::SWGGLSpectrum& response, // query + response
    QString& errorMessage)
{
    (void) errorMessage;
    GLSpectrumSettings settings = m_settings;
    webapiUpdateSpectrumSettings(settings, spectrumSettingsKeys, response);

    MsgConfigureSpectrumVis *msg = MsgConfigureSpectrumVis::create(settings, force);
    m_inputMessageQueue.push(msg);

    if (getMessageQueueToGUI()) // forward to GUI if any
    {
        MsgConfigureSpectrumVis *msgToGUI = MsgConfigureSpectrumVis::create(settings, force);
        getMessageQueueToGUI()->push(msgToGUI);
    }

    webapiFormatSpectrumSettings(response, settings);
    return 200;
}

int SpectrumVis::webapiSpectrumServerGet(SWGSDRangel::SWGSpectrumServer& response, QString& errorMessage) const
{
    (void) errorMessage;
    bool serverRunning = m_wsSpectrum.socketOpened();
    QList<QHostAddress> peerHosts;
    QList<quint16> peerPorts;
    m_wsSpectrum.getPeers(peerHosts, peerPorts);
    response.init();
    response.setRun(serverRunning ? 1 : 0);

    QHostAddress serverAddress = m_wsSpectrum.getListeningAddress();

    if (serverAddress != QHostAddress::Null) {
        response.setListeningAddress(new QString(serverAddress.toString()));
    }

    uint16_t serverPort = m_wsSpectrum.getListeningPort();

    if (serverPort != 0) {
        response.setListeningPort(serverPort);
    }

    if (peerHosts.size() > 0)
    {
        response.setClients(new QList<SWGSDRangel::SWGSpectrumServer_clients*>);

        for (int i = 0; i < peerHosts.size(); i++)
        {
            response.getClients()->push_back(new SWGSDRangel::SWGSpectrumServer_clients);
            response.getClients()->back()->setAddress(new QString(peerHosts.at(i).toString()));
            response.getClients()->back()->setPort(peerPorts.at(i));
        }
    }

    return 200;
}

int SpectrumVis::webapiSpectrumServerPost(SWGSDRangel::SWGSuccessResponse& response, QString& errorMessage)
{
    (void) errorMessage;
    MsgConfigureWSpectrumOpenClose *msg = MsgConfigureWSpectrumOpenClose::create(true);
    m_inputMessageQueue.push(msg);

    if (getMessageQueueToGUI()) // forward to GUI if any
    {
        MsgConfigureWSpectrumOpenClose *msgToGui = MsgConfigureWSpectrumOpenClose::create(true);
        getMessageQueueToGUI()->push(msgToGui);
    }

    response.setMessage(new QString("Websocket spectrum server started"));
    return 200;
}

int SpectrumVis::webapiSpectrumServerDelete(SWGSDRangel::SWGSuccessResponse& response, QString& errorMessage)
{
    (void) errorMessage;
    MsgConfigureWSpectrumOpenClose *msg = MsgConfigureWSpectrumOpenClose::create(false);
    m_inputMessageQueue.push(msg);

    if (getMessageQueueToGUI()) // forward to GUI if any
    {
        MsgConfigureWSpectrumOpenClose *msgToGui = MsgConfigureWSpectrumOpenClose::create(false);
        getMessageQueueToGUI()->push(msgToGui);
    }

    response.setMessage(new QString("Websocket spectrum server stopped"));
    return 200;
}

void SpectrumVis::webapiFormatSpectrumSettings(SWGSDRangel::SWGGLSpectrum& response, const GLSpectrumSettings& settings)
{
    response.setFftSize(settings.m_fftSize);
    response.setFftOverlap(settings.m_fftOverlap);
    response.setFftWindow((int) settings.m_fftWindow);
    response.setRefLevel(settings.m_refLevel);
    response.setPowerRange(settings.m_powerRange);
    response.setDecay(settings.m_decay);
    response.setDecayDivisor(settings.m_decayDivisor);
    response.setHistogramStroke(settings.m_histogramStroke);
    response.setDisplayGridIntensity(settings.m_displayGridIntensity);
    response.setDisplayTraceIntensity(settings.m_displayTraceIntensity);
    response.setDisplayWaterfall(settings.m_displayWaterfall ? 1 : 0);
    response.setInvertedWaterfall(settings.m_invertedWaterfall ? 1 : 0);
    response.setWaterfallShare(settings.m_waterfallShare);
    response.setDisplayMaxHold(settings.m_displayMaxHold ? 1 : 0);
    response.setDisplayCurrent(settings.m_displayCurrent ? 1 : 0);
    response.setDisplayHistogram(settings.m_displayHistogram ? 1 : 0);
    response.setDisplayGrid(settings.m_displayGrid ? 1 : 0);
    response.setAveragingMode((int) settings.m_averagingMode);
    response.setAveragingValue(GLSpectrumSettings::getAveragingValue(settings.m_averagingIndex, settings.m_averagingMode));
    response.setLinear(settings.m_linear ? 1 : 0);
    response.setSsb(settings.m_ssb ? 1 : 0);
    response.setUsb(settings.m_usb ? 1 : 0);
    response.setWsSpectrumPort(settings.m_wsSpectrumPort);

    if (response.getWsSpectrumAddress()) {
        *response.getWsSpectrumAddress() = settings.m_wsSpectrumAddress;
    } else {
        response.setWsSpectrumAddress(new QString(settings.m_wsSpectrumAddress));
    }
}

void SpectrumVis::webapiUpdateSpectrumSettings(
    GLSpectrumSettings& settings,
    const QStringList& spectrumSettingsKeys,
    SWGSDRangel::SWGGLSpectrum& response)
{
    if (spectrumSettingsKeys.contains("fftSize")) {
        settings.m_fftSize = response.getFftSize();
    }
    if (spectrumSettingsKeys.contains("fftOverlap")) {
        settings.m_fftOverlap = response.getFftOverlap();
    }
    if (spectrumSettingsKeys.contains("fftWindow")) {
        settings.m_fftWindow = (FFTWindow::Function) response.getFftWindow();
    }
    if (spectrumSettingsKeys.contains("refLevel")) {
        settings.m_refLevel = response.getRefLevel();
    }
    if (spectrumSettingsKeys.contains("powerRange")) {
        settings.m_powerRange = response.getPowerRange();
    }
    if (spectrumSettingsKeys.contains("decay")) {
        settings.m_decay = response.getDecay();
    }
    if (spectrumSettingsKeys.contains("decayDivisor")) {
        settings.m_decayDivisor = response.getDecayDivisor();
    }
    if (spectrumSettingsKeys.contains("histogramStroke")) {
        settings.m_histogramStroke = response.getHistogramStroke();
    }
    if (spectrumSettingsKeys.contains("displayGridIntensity")) {
        settings.m_displayGridIntensity = response.getDisplayGridIntensity();
    }
    if (spectrumSettingsKeys.contains("displayTraceIntensity")) {
        settings.m_displayTraceIntensity = response.getDisplayTraceIntensity();
    }
    if (spectrumSettingsKeys.contains("displayWaterfall")) {
        settings.m_displayWaterfall = response.getDisplayWaterfall() != 0;
    }
    if (spectrumSettingsKeys.contains("invertedWaterfall")) {
        settings.m_invertedWaterfall = response.getInvertedWaterfall() != 0;
    }
    if (spectrumSettingsKeys.contains("waterfallShare")) {
        settings.m_waterfallShare = response.getWaterfallShare();
    }
    if (spectrumSettingsKeys.contains("displayMaxHold")) {
        settings.m_displayMaxHold = response.getDisplayMaxHold() != 0;
    }
    if (spectrumSettingsKeys.contains("displayCurrent")) {
        settings.m_displayCurrent = response.getDisplayCurrent() != 0;
    }
    if (spectrumSettingsKeys.contains("displayHistogram")) {
        settings.m_displayHistogram = response.getDisplayHistogram() != 0;
    }
    if (spectrumSettingsKeys.contains("displayGrid")) {
        settings.m_displayGrid = response.getDisplayGrid() != 0;
    }
    if (spectrumSettingsKeys.contains("averagingMode")) {
        settings.m_averagingMode = (GLSpectrumSettings::AveragingMode) response.getAveragingMode();
    }
    if (spectrumSettingsKeys.contains("averagingValue"))
    {
        qint32 tmp = response.getAveragingValue();
        settings.m_averagingIndex = GLSpectrumSettings::getAveragingIndex(tmp, settings.m_averagingMode);
        settings.m_averagingValue = GLSpectrumSettings::getAveragingValue(settings.m_averagingIndex, settings.m_averagingMode);
    }
    if (spectrumSettingsKeys.contains("linear")) {
        settings.m_linear = response.getLinear() != 0;
    }
    if (spectrumSettingsKeys.contains("ssb")) {
        settings.m_ssb = response.getSsb() != 0;
    }
    if (spectrumSettingsKeys.contains("usb")) {
        settings.m_usb = response.getUsb() != 0;
    }
    if (spectrumSettingsKeys.contains("wsSpectrumAddress")) {
        settings.m_wsSpectrumAddress = *response.getWsSpectrumAddress();
    }
    if (spectrumSettingsKeys.contains("wsSpectrumPort")) {
        settings.m_wsSpectrumPort = response.getWsSpectrumPort();
    }
}
#ifndef _ADS_NOTIFICATION_H_
#define _ADS_NOTIFICATION_H_

#include "AdsDef.h"
#include "RingBuffer.h"
#include <memory>

struct Notification
{
	const uint16_t port;

	Notification(PAdsNotificationFuncEx __func, uint32_t hNotify, uint32_t __hUser, uint32_t length, AmsAddr __amsAddr, uint16_t __port)
		: port(__port),
		callback(__func),
		buffer(new uint8_t[sizeof(AdsNotificationHeader) + length]),
		hUser(__hUser),
		amsAddr(__amsAddr)
	{
		auto header = reinterpret_cast<AdsNotificationHeader*>(buffer.get());
		header->hNotification = hNotify;
		header->cbSampleSize = length;
	}

	void Notify(uint64_t timestamp, RingBuffer& ring) const
	{
		auto header = reinterpret_cast<AdsNotificationHeader*>(buffer.get());
		for (size_t i = 0; i < header->cbSampleSize; ++i) {
			header->data[i] = ring.ReadFromLittleEndian<uint8_t>();
		}
		header->nTimeStamp = timestamp;
		callback(&amsAddr, header, hUser);
	}

	uint32_t Size() const
	{
		auto header = reinterpret_cast<AdsNotificationHeader*>(buffer.get());
		return header->cbSampleSize;
	}
private:
	const PAdsNotificationFuncEx callback;
	const std::shared_ptr<uint8_t> buffer;
	const uint32_t hUser;
	const AmsAddr amsAddr;
};

#endif /* #ifndef _ADS_NOTIFICATION_H_ */

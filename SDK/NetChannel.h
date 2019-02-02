#pragma once


namespace SDK
{
	class NetChannel
	{
	public:



		uint8_t pad_0x0000[0x17];
		bool m_bShouldDelete;
		int m_nOutSequenceNr;
		int m_nInSequenceNr;
		int m_nOutSequenceNrAck;
		int m_nOutReliableState;
		int m_nInReliableState;
		int m_nChokedPackets;
	};
}
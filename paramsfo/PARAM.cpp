#include "PARAM.h"


PARAM::PARAM(QString path){
	f.setFileName(path);
	f.open(QIODevice::ReadWrite);
	QDataStream ds(&f);
	ds >> sfo;
	for (auto s : sfo.key_table)
		qDebug() << s;
	for (auto s : sfo.data_table)
		qDebug() << s;
}


PARAM::~PARAM() {
	f.close();
}


bool PARAM::isValidParam(QDataStream & in) {
	in.device()->reset();
	in.setByteOrder(QDataStream::BigEndian);
	quint32 signature, version;
	in >> signature >> version;
	return ((signature == 0x00505346) && (version == 0x01010000));
}


QDataStream & operator>>(QDataStream & in, PARAM::SFO & s) {
	in.device()->reset();
	in.setByteOrder(QDataStream::BigEndian);
	in >> s.header.magic >> s.header.version;
	in.setByteOrder(QDataStream::LittleEndian);
	in >> s.header.key_table_start >> s.header.data_table_start >> s.header.tables_entries;
	s.index_table.resize(s.header.tables_entries);
	for (int i = 0; i < s.header.tables_entries; ++i)
		in >> s.index_table[i].key_offset >> s.index_table[i].data_fmt >> s.index_table[i].data_len
		>> s.index_table[i].data_max_len >> s.index_table[i].data_offset;
	for (int i = 0; i < s.header.tables_entries; ++i) {
		quint8 byte;
		QByteArray key;
		do {
			in >> byte;
			key.append(byte);
		} while (byte);
		s.key_table << key;
	}
	in.device()->seek(s.header.data_table_start);
	for (int i = 0; i < s.header.tables_entries; ++i) {
		QByteArray data(s.index_table[i].data_max_len, '\0');
		in.readRawData(data.data(), s.index_table[i].data_max_len);
		s.data_table << data;
	}
	return in;
}


QDataStream & operator<<(QDataStream & out, PARAM::SFO  & s) {
	out.device()->reset();
	out.setByteOrder(QDataStream::BigEndian);
	out << s.header.magic << s.header.version;
	out.setByteOrder(QDataStream::LittleEndian);
	s.header.key_table_start = 0x14 + s.index_table.count() * 0x10;
	s.header.data_table_start = s.header.key_table_start;
	s.header.tables_entries = s.index_table.count();
	for (auto key : s.key_table)
		s.header.data_table_start += key.length();
	if (s.header.data_table_start % 4 != 0)
		s.header.data_table_start = (s.header.data_table_start / 4 + 1) * 4;
	out << s.header.key_table_start << s.header.data_table_start << s.header.tables_entries;
	for (int i = 0; i < s.header.tables_entries; ++i)
		out << s.index_table[i].key_offset << s.index_table[i].data_fmt << s.index_table[i].data_len
		<< s.index_table[i].data_max_len << s.index_table[i].data_offset;
	for (int i = 0; i < s.header.tables_entries; ++i)
		out.writeRawData(s.key_table[i].data(), s.key_table[i].length());
	out.device()->seek(s.header.data_table_start);
	for (int i = 0; i < s.header.tables_entries; ++i)
		out.writeRawData(s.data_table[i].data(), s.data_table[i].length());
	return out;
}


//ACCOUNT_ID 			utf8 - S
//ACCOUNTID 			utf8
//ANALOG_MODE			int32
//APP_VER 				utf8
//ATTRIBUTE 			int32
//BOOTABLE 				int32
//CATEGORY 				utf8
//CONTENT_ID			utf8
//DETAIL 				utf8
//GAMEDATA_ID			utf8
//ITEM_PRIORITY			int32
//LANG 					int32
//LICENSE 				utf8
//NP_COMMUNICATION_ID 	utf8
//NPCOMMID 				utf8
//PADDING 				utf8 - S
//PARAMS 				utf8 - S
//PARAMS2 				utf8 - S
//PATCH_FILE 			utf8
//PS3_SYSTEM_VER 		utf8
//REGION_DENY 			int32
//RESOLUTION 			int32
//SAVEDATA_DETAIL 		utf8
//SAVEDATA_DIRECTORY 	utf8
//SAVEDATA_FILE_LIST 	utf8 - S
//SAVEDATA_LIST_PARAM 	utf8
//SAVEDATA_PARAMS 		utf8 - S
//SAVEDATA_TITLE 		utf8
//SOUND_FORMAT		 	int32
//SOURCE				int32
//SUB_TITLE				utf8
//TARGET_APP_VER		utf8
//TITLE 				utf8
//TITLE_ID				utf8
//PARENTAL_LEVEL 		int32
//PARENTALLEVEL 		int32
//VERSION 				utf8
//XMB_APPS 				int32
//PARENTAL_LEVEL_x 		int32
//TITLE_xx 				utf8
//TITLEID0xx			utf8
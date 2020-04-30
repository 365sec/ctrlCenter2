
#ifndef	SOME_DATA_H
#define SOME_DATA_H

#include "boost/function.hpp"
#include <vector>
#include <iostream>
extern int NetDecryptData(char *pBuf, const int &nLen, char *pOut);
#include "shared_buffer.hpp"
using namespace std;

//ʵ����ճ������

class direct_data{
public:
	typedef boost::function<bool (byte * dat, size_t size)> dealer_t;//TODO
	dealer_t deal_;

	axis::shared_buffer data_;

	bool data_in(byte* dat,size_t length){
        NetDecryptData((char*)dat, length, NULL);
		data_.in(dat,length);
		std::pair<bool,bool> ret;
	    do{
			ret = deal();
		}while(ret.first && ret.second);
		return ret.second;
	 }

	std::pair<bool,bool> deal(){//��1��bool��ʾ�����Ƿ���������2��bool��ʾ�������Ƿ�������Ҫ���ǶϿ�����
		msg_type::head head;
		size_t data_size = data_.size();
		bool ok=true;
		if(data_size>sizeof(head)){
			memcpy(&head, data_.raw_data(), sizeof(head));//ȡͷ�������ΪTLV��ʽ
#ifdef CFG_MDL_UPGRADE_SVR
			if(head.type = 1)
			{
				head.len = data_size;
			}
#endif
			size_t pkg_size =head.len;
			if(pkg_size<=data_size){
				if(deal_){
					axis::shared_buffer sb;
					sb.resize(pkg_size);
					int ret=data_.out(sb.raw_data(),pkg_size);
					if(pkg_size!=ret){
						cout<<"err: ret!=pkg_size"<<endl;
						exit(0);
					}else{
						ok = deal_(sb.raw_data(),pkg_size);
						if(!ok){
							return std::make_pair(false,false);
						}
					}
				}
				return std::make_pair(true,ok);
			}
		}
		return std::make_pair(false,ok);
	}

};

#endif
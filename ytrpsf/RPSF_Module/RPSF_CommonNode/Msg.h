#pragma once
#include <ytlib/NetTools/TcpNetUtil.h>
#include <ytlib/NetTools/TcpNetAdapter.h>

namespace rpsf {

	//���ļ��ж���һЩ���ݰ���ʽ��������Ҫ������ṩ�����ݰ���Ҫ�������ӿ�

	//���ڴ����������
	class rpsfData {
		//ֻ��Ҫ���л�ǰ����
		T_CLASS_SERIALIZE(&m_name&m_sender)
	public:
		rpsfData() {}
		rpsfData(const std::string& name_, const std::string& sender_) :
			m_name(name_), m_sender(sender_),m_delfiles(false) {

		}
		//ֻ��ֱ�Ӵ���boost::shared_array<char>�Ų����п���
		void addData(const std::string& dataTag, const boost::shared_array<char>& buf_, uint32_t buf_size_) {
			map_datas[dataTag] = std::move(ytlib::shared_buf(buf_, buf_size_));
		}
		void addData(const std::string& dataTag, const char* buf_, uint32_t buf_size_) {
			ytlib::shared_buf tmpbuf(buf_size_);
			memcpy(tmpbuf.buf.get(), buf_, buf_size_);
			map_datas[dataTag] = std::move(tmpbuf);
		}
		void addData(const std::string& dataTag, const std::string& data) {
			ytlib::shared_buf tmpbuf(data.size());
			memcpy(tmpbuf.buf.get(), data.c_str(), tmpbuf.buf_size);
			map_datas[dataTag] = std::move(tmpbuf);
		}

		std::string m_name;//��������
		std::string m_sender;//���ݷ����ߣ�������ƣ�

		bool m_delfiles;//������ɺ��Ƿ�ɾ���ļ�
		std::map<std::string, ytlib::shared_buf> map_datas;//����,���֧��255��
		std::map<std::string, std::string> map_files;//�ļ�,���֧��255��
		
	};
	//���ڴ���һ����Ϣ
	class rpsfEvent {
		T_CLASS_SERIALIZE(&m_name&m_sender)
	public:
		rpsfEvent() {}
		rpsfEvent(const std::string& name_, const std::string& sender_) :
			m_name(name_), m_sender(sender_){
		}
		void setEventMsg(const char* buf_, uint32_t buf_size_) {
			msgbuf.buf_size = buf_size_;
			msgbuf.buf = boost::shared_array<char>(new char[buf_size_]);
			memcpy(msgbuf.buf.get(), buf_, buf_size_);
		}
		void setEventMsg(const std::string& eventmsg) {
			msgbuf.buf_size = eventmsg.size();
			msgbuf.buf = boost::shared_array<char>(new char[msgbuf.buf_size]);
			memcpy(msgbuf.buf.get(), eventmsg.c_str(), msgbuf.buf_size);
		}

		std::string m_name;//��������
		std::string m_sender;//���ݷ����ߣ�������ƣ�

		ytlib::shared_buf msgbuf;

	};
	//����RPC����
	class rpsfRpc {

	public:


	};
	//����RPC���ؽ��
	class rpsfRRpc {

	public:

	};
	//����ϵͳ��Ϣ
	class rpsfSys {

	public:

	};


	//��ײ����ݰ���ʽ
	class rpsfMsg {
		T_CLASS_SERIALIZE(&m_srcAddr&m_msgType&emcy&p_rpsfData&p_rpsfEvent&p_rpsfRpc&p_rpsfRRpc&p_rpsfSys)
	public:
		enum MsgType{
			RPSF_DATA,
			RPSF_EVENT,
			RPSF_RPC,
			RPSF_RRPC,
			RPSF_SYS
		};
		rpsfMsg(){}
		rpsfMsg(uint32_t myid_, MsgType type_, bool is_emcy = false) :
			m_srcAddr(myid_), emcy(is_emcy), m_msgType(type_) {	}

		uint32_t m_srcAddr;//��ϢԴ���id
		MsgType m_msgType;//��Ϣ����
		bool emcy;//�Ƿ�Ϊ��������

		//��boost::shared_ptr��װҪʹ�õ��ϲ����һ��shared_ptrֻ������2�ֽڣ�binģ�ͣ���3�ֽڣ�textģʽ��
		boost::shared_ptr<rpsfData> p_rpsfData;
		boost::shared_ptr<rpsfEvent> p_rpsfEvent;
		boost::shared_ptr<rpsfRpc> p_rpsfRpc;
		boost::shared_ptr<rpsfRRpc> p_rpsfRRpc;
		boost::shared_ptr<rpsfSys> p_rpsfSys;
		
	};
	typedef ytlib::DataPackage<rpsfMsg> rpsfDataPackage;

}



#pragma once
#include <ytlib/NetTools/TcpNetUtil.h>
#include <ytlib/NetTools/TcpNetAdapter.h>
#include <boost/date_time/posix_time/posix_time.hpp>  


namespace rpsf {

	//���ļ��ж���һЩ���ݰ���ʽ��������Ҫ������ṩ�����ݰ���Ҫ�������ӿ�

	enum MsgType {
		RPSF_DATA = 0,
		RPSF_EVENT = 1,
		RPSF_RPC = 2,
		RPSF_RRPC = 3,
		RPSF_SYS = 4,

		RPSF_HANDLETYPE_ORDER = 0,
		RPSF_HANDLETYPE_UNORDER = 16,
		RPSF_HANDLETYPE_EMCY = 32
	};


	//���ڴ����������
	class rpsfDataMsg {
		//ֻ��Ҫ���л�ǰ����
		T_CLASS_SERIALIZE(&m_name&m_sender)
	public:
		rpsfDataMsg() {}
		rpsfDataMsg(const std::string& name_, const std::string& sender_) :m_name(name_), m_sender(sender_),m_delfiles(false),
			m_msgType(MsgType::RPSF_DATA| MsgType::RPSF_HANDLETYPE_UNORDER){

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
		void addData(const std::string& dataTag, const std::string& data_) {
			ytlib::shared_buf tmpbuf(data_.size());
			memcpy(tmpbuf.buf.get(), data_.c_str(), tmpbuf.buf_size);
			map_datas[dataTag] = std::move(tmpbuf);
		}

		std::string m_name;//��������
		std::string m_sender;//���ݷ����ߣ�������ƣ�


		std::map<std::string, ytlib::shared_buf> map_datas;//����,���֧��255��
		std::map<std::string, std::string> map_files;//�ļ�,���֧��255��
		
		uint8_t m_msgType;
		bool m_delfiles;//������ɺ��Ƿ�ɾ���ļ�
	};
	//���ڴ��䵥����Ϣ
	class rpsfEventMsg {
		T_CLASS_SERIALIZE(&m_name&m_sender&m_eventTime)
	public:
		rpsfEventMsg() {}
		rpsfEventMsg(const std::string& name_, const std::string& sender_) :m_name(name_), m_sender(sender_), 
			m_msgType(MsgType::RPSF_EVENT | MsgType::RPSF_HANDLETYPE_ORDER) {
		}
		void setEventMsg(const boost::shared_array<char>& buf_, uint32_t buf_size_) {
			map_datas["e"] = std::move(ytlib::shared_buf(buf_, buf_size_));
		}

		void setEventMsg(const char* buf_, uint32_t buf_size_) {
			ytlib::shared_buf tmpbuf(buf_size_);
			memcpy(tmpbuf.buf.get(), buf_, buf_size_);
			map_datas["e"] = std::move(tmpbuf);
		}
		void setEventMsg(const std::string& eventmsg) {
			ytlib::shared_buf tmpbuf(eventmsg.size());
			memcpy(tmpbuf.buf.get(), eventmsg.c_str(), tmpbuf.buf_size);
			map_datas["e"] = std::move(tmpbuf);
		}

		void setEventTime() {
			boost::posix_time::ptime tnow = boost::posix_time::microsec_clock::universal_time();
			memcpy(&m_eventTime, &tnow, 8);
		}
		std::string getEventTime() {
			boost::posix_time::ptime pt;
			memcpy(&pt, &m_eventTime, 8);
			return boost::posix_time::to_iso_string(pt);
		}

		std::string m_name;//��������
		std::string m_sender;//���ݷ����ߣ�������ƣ�
		uint64_t m_eventTime;

		std::map<std::string, ytlib::shared_buf> map_datas;//����,���֧��255��

		uint8_t m_msgType;
	};
	//����RPC���á�����֧���ļ�
	class rpsfRpc {
		T_CLASS_SERIALIZE(&m_service&m_fun&m_rpcID)
	public:
		rpsfRpc(){}
		rpsfRpc(const std::string& service_, const std::string& fun_) :
			m_service(service_), m_fun(fun_), m_msgType(MsgType::RPSF_RPC | MsgType::RPSF_HANDLETYPE_UNORDER) {

		}

		std::string m_service;//Զ�̲��������
		std::string m_fun;//Զ�̲���ķ���
		uint32_t m_rpcID;//rpc�ı�š�Ӧ��֤һ������ڵ�rpc��Ż���һ��

		uint32_t m_timeout;//��ʱʱ�䡣0��ʾһֱ�ȴ�

		std::map<std::string, ytlib::shared_buf> map_datas;//����,���֧��255��
		std::map<std::string, std::string> map_files;//�ļ�,���֧��255��

		uint8_t m_msgType;
		bool m_delfiles;//������ɺ��Ƿ�ɾ���ļ�
	};
	//����RPC���ؽ��
	class rpsfRRpc {

	public:
		uint32_t m_rpcID;//��Ӧ��rpc�ı��
		uint32_t m_errorCode;//������

	};

	//����ϵͳ��Ϣ�����ṩ��������ã�һ�����ڴ��ݸ��ֽṹ��
	class rpsfSys {
		T_CLASS_SERIALIZE(&m_sysMsgType)
	public:
		//ϵͳ��Ϣ����
		enum {
			RPSF_SYS_NEWNODE
		};
		rpsfSys() :m_msgType(MsgType::RPSF_SYS | MsgType::RPSF_HANDLETYPE_EMCY) {

		}

		uint8_t m_sysMsgType;//��Ϣ����


		std::map<std::string, ytlib::shared_buf> map_datas;//����,���֧��255��

		uint8_t m_msgType;
	};


	//��ײ����ݰ���ʽ
	class rpsfMsg {
		T_CLASS_SERIALIZE(&m_srcAddr&m_msgType&emcy&p_rpsfData&p_rpsfEvent&p_rpsfRpc&p_rpsfRRpc&p_rpsfSys)
	public:

		rpsfMsg(){}
		rpsfMsg(uint32_t myid_, uint8_t type_) :m_srcAddr(myid_), m_msgType(type_) {}

		uint32_t m_srcAddr;//��ϢԴ���id
		uint8_t m_msgType;//��Ϣ���͡���ΪҪ֧�����л���������Ϊint


		//��boost::shared_ptr��װҪʹ�õ��ϲ����һ��shared_ptrֻ������2�ֽڣ�binģ�ͣ���3�ֽڣ�textģʽ��
		boost::shared_ptr<rpsfDataMsg> p_rpsfData;
		boost::shared_ptr<rpsfEventMsg> p_rpsfEvent;
		boost::shared_ptr<rpsfRpc> p_rpsfRpc;
		boost::shared_ptr<rpsfRRpc> p_rpsfRRpc;
		boost::shared_ptr<rpsfSys> p_rpsfSys;
		
	};
	typedef ytlib::DataPackage<rpsfMsg> rpsfDataPackage;

}



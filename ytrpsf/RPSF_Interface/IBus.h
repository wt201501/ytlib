#pragma once
#include <map>
#include <set>
#include <boost/shared_array.hpp>

namespace rpsf {
	//���ļ��ṩbus�Ľӿ�
	enum handleType {
		RPSF_ORDER,
		RPSF_UNORDER,
		RPSF_EMCY
	};
	/*���ݰ����ࡣע�⣺
	1����������Ķ�����Ҫʹ��getCopy����������ֻ�����Ӷ���ͬ�����ݵ�����
	2���Դ���д�����̰߳�ȫ
	3���ڵ���ibus�ӿڽ���Щ�����������ߺ��緢���¼������ݡ�RPC���ã�������һ��ʱ��Ķ���lock���ڴ��ڼ��д��������������
	*/
	class rpsfPackage {
	public:
		rpsfPackage();
		rpsfPackage getCopy() const;

		void addData(const std::string& dataTag, const boost::shared_array<char>& buf_, uint32_t buf_size_);
		void addData(const std::string& dataTag, const char* buf_, uint32_t buf_size_);
		void addData(const std::string& dataTag, const std::string& data_);
		void delData(const std::string& dataTag);
		void clearDatas();

		void addFile(const std::string& fileTag, const std::string& filePath_);
		void delFile(const std::string& dataTag);
		void clearFiles();

		bool getData(const std::string& dataTag, boost::shared_array<char>& buf_, uint32_t& buf_size_) const;
		std::set<std::string> getDataList() const;
		bool getFile(const std::string& fileTag, std::string& filePath_) const;
		std::set<std::string> getFileList() const;

		void setIfDelFiles(bool delfiles_);
		bool getIfDelFiles() const;

		void setHandleType(handleType type_);
		handleType getHandleType() const;
	protected:
		std::shared_ptr<void> p;
	};

	//���ݰ���������������
	class rpsfData : public rpsfPackage {
	public:
		explicit rpsfData(const std::string& name_, const std::string& sender_);
		std::string getDataName() const;
		std::string getSender() const;

	};
	//�¼��������������¼�
	class rpsfEvent : private rpsfPackage {
	public:
		explicit rpsfEvent(const std::string& name_, const std::string& sender_);
		std::string getEventName() const;
		std::string getSender() const;
		void setEventTime();
		std::string getEventTime() const;

		void setEventMsg(const boost::shared_array<char>& buf_, uint32_t buf_size_);
		void setEventMsg(const char* buf_, uint32_t buf_size_);
		void setEventMsg(const std::string& eventmsg);
		void clearEventMsg();
		void getEventMsg(boost::shared_array<char>& buf_, uint32_t& buf_size_) const;

	};
	//RPC���ð��������洢RPC����
	class rpsfCallArgs : public rpsfPackage {
	public:
		explicit rpsfCallArgs(const std::string& service_, const std::string& fun_, uint32_t timeout = 0);
		std::string getServiceName() const;
		std::string getFunName() const;
		uint32_t getTimeOut() const;

	};
	//RPC���÷��ذ��������洢RPC���
	class rpsfResult : public rpsfPackage {
	public:
		explicit rpsfResult();
		operator bool() const;
		std::string getErrorInfo() const;

		void setErrorInfo(const std::string& err_);//RPCִ�з����������Ϣ�����ַ�����ʾ�޴���
	};

	//��bus�Ľӿڷ�װ���ṩ�����ʹ��
	class IBus {
	public:
		IBus(void* p_);
		//���ġ�ȡ���������ݡ��¼�
		bool SubscribeData(const std::string& pluginName_, const std::set<std::string>& dataNames_);
		bool UnsubscribeData(const std::string& pluginName_, const std::set<std::string>& dataName_);
		bool SubscribeEvent(const std::string& pluginName_, const std::set<std::string>& eventName_);
		bool UnsubscribeEvent(const std::string& pluginName_, const std::set<std::string>& eventName_);

		
		rpsfResult Invoke(const rpsfCallArgs& callArgs_);//RPC�ӿ�
		bool PublishEvent(const rpsfEvent& event_);//�����¼�
		bool PublishData(const rpsfData& data_);//��������

		//��ȡ���ա�����·��
		std::string GetSendFilePath();
		std::string GetRecvFilePath();

	private:
		void * p;
	};

}
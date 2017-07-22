#pragma once
#include <ytlib/ProcessManager/ProcessBase.h>
#include <ytlib/SupportTools/ChannelBase.h>
#include <ytlib/Common/TString.h>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <shared_mutex>
#include <mutex> 

namespace ytlib {

	/*
	������������
	ʹ��ʱ��Ҫ����һ������id����������Ϣ��map��Ȼ����ʱֻ�贫�����ݺ�Ŀ�ĵ�id����

	ʹ��ͨ�����д�����һ�����ţ�
	*/



	//����������ģ�壬�첽����
	template<class _HostInfo,//������Ϣ��ʽ
	class _TransData,
	class _RecvData = _TransData> //Ĭ�Ͻ������ݸ�ʽ�뷢�����ݸ�ʽ��ͬ
	class NetAdapterBase : public ProcessBase {
	public:

		typedef std::pair<_TransData, std::vector<uint32_t>> _TransData_dst;//���ݼ���Ŀ�ĵ��б�֧��Ⱥ�����ظ�����

		/*������
		myid_������id
		hostInfo_������������Ϣ
		recvcb_���������ݻص�
		�����ͨ����ʼ�����Ըģ�
		*/
		NetAdapterBase(uint32_t myid_, const _HostInfo & hostInfo_,
			std::function<void(_RecvData &)> recvcb_):
			ProcessBase(),
			m_myid(myid_),
			m_bStopFlag(false),
			recvcallback(recvcb_)
		{
			m_mapHostInfo[m_myid] = hostInfo_;
		};
		virtual ~NetAdapterBase() {
			stop();
		}

		virtual bool init() {
			m_TChannelPtr = std::shared_ptr<ChannelBase<_TransData_dst>>(
				new ChannelBase<_TransData_dst>(std::bind(&NetAdapterBase::_sendfun, this, std::placeholders::_1), 1, 1000));
			m_RChannelPtr = std::shared_ptr<ChannelBase<_RecvData>>(
				new ChannelBase<_RecvData>(recvcallback, 1, 1000));
			return ProcessBase::init();
		}

		//�ṩ�����ͷ��ķ��ͽӿ�
		inline bool Send(const _TransData & Tdata_,const std::vector<uint32_t>& dst_) {
			return is_running && m_TChannel.Add(std::move(std::make_pair<_TransData, std::vector<uint32_t>>(Tdata_, dst_)));
		}
		
		//hostinfo����
		inline _HostInfo GetMyHostInfo() {
			std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
			return m_mapHostInfo[m_myid];
		}
		inline _HostInfo GetHostInfo(uint32_t hostid_) {
			std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
			std::map<uint32_t, _HostInfo>::iterator itr = m_mapHostInfo.find(hostid_);
			if (itr != m_mapHostInfo.end()) return itr->second;
			else return _HostInfo();//���쳣��
		}
		//��������info�����򸲸ǣ��������
		bool SetHost(uint32_t hostid_, const _HostInfo & hostInfo_) {
			std::unique_lock<std::shared_mutex> lck(m_hostInfoMutex);
			m_mapHostInfo[hostid_] = hostInfo_;
		}
		//�Ƴ�����info�������Ƴ�����info
		bool RemoveHost(uint32_t hostid_) {
			if (hostid_ == m_myid) return false;
			std::unique_lock<std::shared_mutex> lck(m_hostInfoMutex);
			std::map<uint32_t, _HostInfo>::iterator itr = m_mapHostInfo.find(hostid_);
			if (itr == m_mapHostInfo.end()) return false;
			m_mapHostInfo.erase(itr);
			return true;
		}


	protected:
		//�ṩ�������̵߳Ľ������ݽӿڡ����������
		inline bool Recv(const _RecvData & Rdata_) {
			return is_running && m_RChannel.Add(Rdata_);
		}
		//���ͺ���
		virtual void _sendfun(const _TransData_dst& Tdata_) = 0;


	protected:
		const uint32_t m_myid;//����id������֮���޷��޸�

		std::shared_mutex m_hostInfoMutex;//�����б�Ķ�д��
		std::map<uint32_t, _HostInfo> m_mapHostInfo;//�����б�id-info

		std::function<void(_RecvData &)> m_receiveCallBack;//�ص�
		std::atomic_bool m_bStopFlag;

		std::shared_ptr<ChannelBase<_TransData_dst>> m_TChannelPtr;//����ͨ��
		std::shared_ptr<ChannelBase<_RecvData>> m_RChannelPtr;//����ͨ��
		std::function<void(_RecvData &)> recvcallback;//���ջص�����
	};
}
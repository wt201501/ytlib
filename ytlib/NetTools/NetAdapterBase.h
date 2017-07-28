#pragma once

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

	*/

	//����������ģ��
	template<class _HostInfo,//������Ϣ��ʽ
		class _TransData,
		class _RecvData = _TransData> //Ĭ�Ͻ������ݸ�ʽ�뷢�����ݸ�ʽ��ͬ
	class NetAdapterBase {
	public:

		/*������
		myid_������id
		hostInfo_������������Ϣ
		recvcb_���������ݻص���Ӧ���Ƿ��������������ص�
		�����ͨ����ʼ�����Ըģ�
		*/
		NetAdapterBase(uint32_t myid_, const _HostInfo & hostInfo_,
			std::function<void(_RecvData &)> recvcb_):
			m_myid(myid_),
			m_receiveCallBack(recvcb_)
		{
			m_mapHostInfo[m_myid] = hostInfo_;
		};
		virtual ~NetAdapterBase() {

		}

		//�ṩ�����ͷ��ķ��ͽӿ�
		virtual bool Send(const _TransData & Tdata_, uint32_t dst_) {
			if (dst_ == m_myid) return false;
			std::vector<_HostInfo> vec_hostinfo;
			m_hostInfoMutex.lock_shared();
			std::map<uint32_t, _HostInfo>::const_iterator itr = m_mapHostInfo.find(dst_);
			if (itr == m_mapHostInfo.end()) return false;
			vec_hostinfo.push_back(itr->second);
			m_hostInfoMutex.unlock_shared();
			return _sendfun(Tdata_, vec_hostinfo);
		}

		virtual bool Send(const _TransData & Tdata_,const std::vector<uint32_t>& dst_) {
			std::vector<_HostInfo> vec_hostinfo;
			size_t size = dst_.size();
			if (size == 0) return false;
			m_hostInfoMutex.lock_shared();
			for (size_t ii = 0; ii < size; ++ii) {
				if (dst_[ii] == m_myid) return false;
				std::map<uint32_t, _HostInfo>::const_iterator itr = m_mapHostInfo.find(dst_[ii]);
				if (itr == m_mapHostInfo.end()) return false;
				vec_hostinfo.push_back(itr->second);
			}
			m_hostInfoMutex.unlock_shared();
			return _sendfun(Tdata_, vec_hostinfo);
		}
		
		//hostinfo����
		inline _HostInfo GetMyHostInfo() {
			std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
			return m_mapHostInfo[m_myid];
		}
		inline _HostInfo GetHostInfo(uint32_t hostid_) {
			std::shared_lock<std::shared_mutex> lck(m_hostInfoMutex);
			std::map<uint32_t, _HostInfo>::const_iterator itr = m_mapHostInfo.find(hostid_);
			if (itr != m_mapHostInfo.end()) return itr->second;
			else return _HostInfo();//���쳣��
		}
		//��������info�����򸲸ǣ��������
		bool SetHost(uint32_t hostid_, const _HostInfo & hostInfo_) {
			std::unique_lock<std::shared_mutex> lck(m_hostInfoMutex);
			m_mapHostInfo[hostid_] = hostInfo_;
			return true;
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
		//���ͺ���
		virtual bool _sendfun(const _TransData & Tdata_, const std::vector<_HostInfo>& dst_) = 0;

		const uint32_t m_myid;//����id������֮���޷��޸�
		std::shared_mutex m_hostInfoMutex;//�����б�Ķ�д��
		std::map<uint32_t, _HostInfo> m_mapHostInfo;//�����б�id-info
		std::function<void(_RecvData &)> m_receiveCallBack;//�ص���ֱ�ӹ��������

	};
}
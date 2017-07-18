#pragma once
#include <ytlib/FileManager/FileBase.h>
#include <ytlib/SupportTools/XMLTools.h>

namespace wtlib
{

	//root�ڵ㣺�������ơ��汾��settings�ڵ㣬item�ڵ㣨�����
	//settings�ڵ㣺key-value��
	//item�ڵ�:���ơ����ͣ�settings�ڵ㡣���Ƽ�Ϊ·����

	/*
	ʾ��xml��
<?xml version="1.0" encoding="utf-8"?>
<Project Name="testprj1" Version="0.0.1">
	<Settings>
		<setting key="key1" value="value1" />
		<setting key="key2" value="value2" />
		<setting key="key3" value="value3" />
	</Settings>
	<ItemGroup>
		<Item itemname="file1.txt" type="type1">
			<Settings>
				<setting key="key1" value="value1" />
				<setting key="key2" value="value2" />
			</Settings>
		</Item>
		<Item itemname="file2.txt" type="type2">
			<Settings>
				<setting key="key1" value="value1" />
				<setting key="key2" value="value2" />
			</Settings>
		</Item>
	</ItemGroup>
</Project>
	*/
	struct ItemInfo {
		tstring type;
		std::map<tstring, tstring> settings;
	};

	struct PrjObj {
		tstring m_PrjName;
		tstring m_PrjVersion;
		std::map<tstring, tstring> m_PrjSettings;//key-value
		std::map<tstring, ItemInfo> m_PrjItems;//path-obj
	};

	//ʹ��xml��ʽ
	class PrjFile :public FileBase<PrjObj>
	{
	public:
		PrjFile() :FileBase() , m_version(WT_TEXT("0.0.1")){}
		virtual ~PrjFile() {}

		inline tstring getPrjName()const { return m_fileobj->m_PrjName; }
		bool setPrjName(const tstring& name_) { 
			if (name_.empty()) return false;
			m_fileobj->m_PrjName = name_;
			return true;
		}

		//versionӦ���������ʼ��ʱȷ���ġ�����ļ����������version��������Ҫ����
		inline tstring getPrjVersion()const { return m_version; }

		inline std::map<tstring, tstring> getPrjSettings() const { return m_fileobj->m_PrjSettings; }
		inline void setPrjSettings(const std::map<tstring, tstring>& setting_) { m_fileobj->m_PrjSettings = setting_; }

		ItemInfo getPrjItem(const tstring& itemname_) const {
			std::map<tstring, ItemInfo>::const_iterator itr = m_fileobj->m_PrjItems.find(itemname_);
			if (itr != m_fileobj->m_PrjItems.end()) return itr->second;
			else return ItemInfo();
		}
		//����ļ�Ŀ¼�Ƿ��ڹ���Ŀ¼�¡����ж��ļ��Ƿ���ڡ�ֻ��ʹ�����·��������������ľ͸��ǵ�
		bool setPrjItem(const tstring& itemname_, const ItemInfo& obj_) {
			if(itemname_.empty()) return false;
			if (tpath(itemname_).is_absolute()) return false;
			m_fileobj->m_PrjItems[itemname_] = obj_;
			return true;
		}
		bool delPrjItem(const tstring& itemname_) {
			std::map<tstring, ItemInfo>::const_iterator itr = m_fileobj->m_PrjItems.find(itemname_);
			if (itr == m_fileobj->m_PrjItems.end()) return false;
			m_fileobj->m_PrjItems.erase(itr);
			return true;
		}
		//��ȡһ�����͵�����Ŀ
		std::vector<tstring> getTypeItem(const tstring& type_) const {
			std::vector<tstring> re;
			for (std::map<tstring, ItemInfo>::const_iterator itr = m_fileobj->m_PrjItems.begin();
			itr != m_fileobj->m_PrjItems.end(); ++itr) {
				if (itr->second.type == type_) re.push_back(itr->first);
			}
			return re;
		}

		//��ȡ����·������items�������·��
		inline tstring getPrjPath() const { return GetFileParentPath();}
		//��ȡ����·��
		inline tstring getItemAbsolutePath(const tstring& path_) {
			return tCombinePath(GetFileParentPath(), path_).string<tstring>();
		}
	protected:
		const tstring m_version;
		virtual std::shared_ptr<PrjObj> GetFileObjPtr() const{
			return FileBase::GetFileObjPtr();
		}
		virtual bool CheckFileName(const tstring& filename) const {
			tstring Suffix1 = WT_TEXT("xml");
			tstring Suffix2 = WT_TEXT("prj");
			if ((ToLower(filename.substr(filename.length() - Suffix1.length(), Suffix1.length())) != Suffix1)
				&& (ToLower(filename.substr(filename.length() - Suffix2.length(), Suffix2.length())) != Suffix2)) {
				return false;
			}
			return true;
		}
		virtual bool CreateFileObj() {
			m_fileobj = std::shared_ptr<PrjObj>(new PrjObj());
			if (m_filepath.empty()) {
				m_fileobj->m_PrjName = WT_TEXT("NewProject");
			}
			else {
				m_fileobj->m_PrjName = GetFileName();
			}
			//�ڴ˴�ȷ��version������̳�ʱҲ�ڴ�
			m_fileobj->m_PrjVersion = m_version;
			return true;
		}
		virtual bool GetFileObj() {
			if (!CreateFileObj()) return false;
			tpath path = tGetAbsolutePath(m_filepath);

			tptree ptRoot;
			if (!realXml(path.string<tstring>(), ptRoot)) return false;
			//����,ͬʱ��Ҫ���

			try {
				tptree ptprj = ptRoot.get_child(WT_TEXT("Project"));
				//�ж�version�Ƿ����
				if (m_version != ptprj.get<tstring>(WT_TEXT("<xmlattr>.Version"))) {
					tcout << WT_TEXT("Version not matched : ") << m_version << WT_TEXT(" VS ") << ptprj.get<tstring>(WT_TEXT("<xmlattr>.Version")) << std::endl;
					return false;
				}
				m_fileobj->m_PrjName = ptprj.get<tstring>(WT_TEXT("<xmlattr>.Name"));
				if (m_fileobj->m_PrjName.empty()) {
					tcout << WT_TEXT("Invalid Project Name!") << std::endl;
					return false;
				}
				m_fileobj->m_PrjVersion = m_version;
				//settings ��ѡ
				if (!readSettings(ptprj, m_fileobj->m_PrjSettings)) return false;
				//ItemGroup ��ѡ
				boost::optional<tptree&> ptItemGroup = ptprj.get_child_optional(WT_TEXT("ItemGroup"));
				if (ptItemGroup) {
					for (tptree::iterator itrptig = ptItemGroup->begin(); itrptig != ptItemGroup->end(); ++itrptig) {
						tptree &Itempt = itrptig->second;
						tstring itemname(Itempt.get<tstring>(WT_TEXT("<xmlattr>.itemname")));
						if (itemname.empty()) {
							tcout << WT_TEXT("Invalid Item Name!") << std::endl;
							return false;
						}
						ItemInfo tmpiteminfo;
						tmpiteminfo.type = Itempt.get<tstring>(WT_TEXT("<xmlattr>.type"));
						if (!readSettings(Itempt, tmpiteminfo.settings)) return false;
						m_fileobj->m_PrjItems[itemname] = tmpiteminfo;
					}
				}
			}
			catch (const std::exception& e) {
				std::cout << "load Project file failed : " << e.what() << std::endl;
				return false;
			}

			return true;
		}
		virtual bool SaveFileObj() {
			tptree ptroot;
			//Ĭ��֮ǰ�����Ч�������ֱ��д��
			tptree ptprj;
			ptprj.put(WT_TEXT("<xmlattr>.Name"), m_fileobj->m_PrjName);
			ptprj.put(WT_TEXT("<xmlattr>.Version"), m_fileobj->m_PrjVersion);
			if (!writeSettings(m_fileobj->m_PrjSettings, ptprj)) return false;
			std::map<tstring, ItemInfo> &tmpiteminfo = m_fileobj->m_PrjItems;
			if (tmpiteminfo.size() > 0) {
				tptree ptItemGroup;
				for (std::map<tstring, ItemInfo>::const_iterator itr = tmpiteminfo.begin();
					itr != tmpiteminfo.end(); ++itr) {
					tptree ptitem;
					ptitem.put(WT_TEXT("<xmlattr>.itemname"), itr->first);
					ptitem.put(WT_TEXT("<xmlattr>.type"), itr->second.type);
					if(!writeSettings(itr->second.settings, ptitem))  return false;
					ptItemGroup.add_child(WT_TEXT("Item"), ptitem);
				}
				ptprj.put_child(WT_TEXT("ItemGroup"), ptItemGroup);
			}
			ptroot.add_child(WT_TEXT("Project"), ptprj);
			return writeXml(m_filepath, ptroot);
		}
	};

}
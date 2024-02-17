#include "commandline.h"
#include "../../log/logger.h"
#include "user.h"
#include <exception>
#include <fmt/format.h>

static std::string cmd_list(const std::vector<std::string>& args,int i)
{
	++i;
	if (args.at(i) == "--teacher")
	{
		//�г�������ʦ����Ϣ
		auto info_list = Account::AccountManager::GetAllTeacherInfo();
		std::string result="ID\tName\tSex\tPhone\tEmail\tCollege\tPermission\tNotes\tEnable\n";
		for (auto& it : info_list)
		{
			result += fmt::format("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n",
				it.id, it.name, it.sex, it.phone_number, it.email, it.college,
				it.permission_level, it.notes, it.is_enable);
		}
		return result;
	}
	else if (args.at(i) == "--student")
	{
		//�г�����ѧ������Ϣ
		auto info_list = Account::AccountManager::GetAllStudentInfo();
		std::string result = "ID\tName\tSex\tPhone\tEmail\tCollege\tEnrollmentDate\tPermission\tNotes\tEnable\n";
		for (auto& it : info_list)
		{
			result += fmt::format("{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n",
								it.id, it.name,it.sex,it.phone_number,it.email,it.college,
				it.enrollment_date,it.permission_level,it.notes,it.is_enable);
			return result;
		}
	}
	else if (args.at(i) == "--subject")
	{
		//�г����пγ̵���Ϣ
		auto info_list = Account::SubjectManager::GetAllSubjectInfo();
		std::string result = "ID\tName\tDescription\n";
		for (auto& it : info_list)
		{
			result += fmt::format("{}\t{}\t{}\n", it.id, it.name, it.description);
		}
		return result;
	}
	else if (args.at(i) == "--class")
	{
		//�г����а༶����Ϣ
		
	}
	else if (args.at(i) == "--user")
	{
		//�г������û���Ϣ
	}
	else if (args.at(i) == "-i")
	{
		//��ʾָ��id����Ϣ
	}
	else if (args.at(i) == "-h")
	{

	}
	else
	{
		return "Unknown option";
	}
}

std::string CommandLine::Execute(const std::vector<std::string>& args)
{
	if (args.size() == 0) return "No command";
	try
	{
		if (args.at(0) == "list") return cmd_list(args, 0);
		else return "Unknown command";
	}
	catch (const std::exception& e)
	{
		return "Execute failed";
	}
}

const std::vector<std::string> CommandLine::Parse(const std::string& command)
{
	std::vector<std::string> args;
	std::string arg;
	bool is_in = false;//��ʶ�Ƿ�����Ч����������
	bool q = false;//��ʶ�Ƿ���˫��������Ĳ�����

	for (auto it : command)
	{
		if (q == true)
		{
			//��ǰ��������
			if (it == '\"')
			{
				//�뿪��������
				q = false;
			}
			else arg.push_back(it);
		}
		else
		{
			//��ǰ����������
			if (it == ' ')
			{
				if (is_in == true)
				{
					//��ֹ��Ч���ֽ���
					is_in = false;
					if (arg != "") args.push_back(arg);
					arg.clear();
				}
			}
			else
			{
				//������Ч��������
				is_in = true;
				if (it == '\"') q = true;//������������
				else arg.push_back(it);
			}
		}
	}
	if (!arg.empty()) args.push_back(arg);
	return args;
}

#include "login.h"
#include "user.h"
#include <map>
#include <exception>
#include "generators.h"
#include "../../log/logger.h"
#include <fmt/format.h>

auto Account::LoginManager::Login(std::uint64_t ID, const std::string& password) -> User
{
	int user_permission_level = 4;
	std::string user_pwd;
	if (password.empty()) throw LoginManagerException("δ�ύ����");
	try
	{
		if (ID >= Generator::STU_ID_START && ID <= Generator::STU_ID_END)
			user_pwd = Account::AccountManager::GetStudentPassword(ID), user_permission_level = 4;
		else if (ID >= Generator::TEACHER_ID_START && ID <= Generator::TEACHER_ID_END)
			user_pwd = Account::AccountManager::GetTeacherPassword(ID), user_permission_level = 2;
		else throw LoginManagerException("�û������������");
		if (user_pwd!=password) throw LoginManagerException("�û������������");
	}
	catch (const Account::AccountException& ex)
	{
		throw LoginManagerException("�û������������");
	}
	catch (const std::exception& ex)
	{
		Logger::LogWarn(fmt::format("�û�{}��¼ʱ�����쳣: {}", ID, ex.what()).c_str());
		throw LoginManagerException("�������ڲ�����");
	}
	//������ȷ�������û�����
	User user();
}

Account::LoginManagerException::LoginManagerException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Account::LoginManagerException::what(void) const noexcept
{
	return this->reason.c_str();
}

#include "user.h"
#include <shared_mutex>
#include "../../rbslib/Storage.h"
#include "../../rbslib/FileIO.h"
#include "../../json/CJsonObject.h"

std::shared_mutex Global_Student_Mutex;//��дѧ����Ϣ�ļ�ʱ����
std::shared_mutex Global_Teacher_Mutex;//��д��ʦ��Ϣ�ļ�ʱ����
std::shared_mutex Global_Classes_Mutex;//��д�༶��Ϣ�ļ�ʱ����

void Account::AccountManager::CreateStudent(std::uint64_t ID, const std::string& name, const std::string& phone_number, const std::string& enrollment_date, const std::string& pass_word, const std::string & college, const std::string& class_name, int permission_level)
{
	//��ѧ����Ϣ����� Students Ŀ¼�£���ѧ����������100000.json
	RbsLib::Storage::StorageFile dir("Students");
	RbsLib::Storage::StorageFile now_dir(".");
	if (dir.IsExist() == false) now_dir.CreateDir("Students");
	if (dir[std::to_string(ID) + ".json"].IsExist()) throw AccountException("Student already exist");
	auto fp = dir[std::to_string(ID) + ".json"].Open(RbsLib::Storage::FileIO::OpenMode::Write |
		RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	obj.Add("Name", name);
	obj.Add("PhoneNumber", phone_number);
	obj.Add("EnrollmentDate", enrollment_date);
	obj.AddEmptySubArray("Subjects");
	obj.Add("College", college);
	obj.Add("Class", class_name);
	obj.Add("Password", pass_word);
	obj.Add("PermissionLevel", permission_level);
	obj.Add("IsEnable", true);
	std::string str = obj.ToFormattedString();
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

void Account::AccountManager::CreateTeacher(std::uint64_t ID, const std::string& name, const std::string& phone_number, const std::string& pass_word, const std::string& class_name, int permission_level)
{
	//����ʦ��Ϣ����� Teachers Ŀ¼�£��Թ�����������10000.json
}

void Account::AccountManager::AddSubjectToStudent(std::uint64_t student_id, std::uint64_t subject_id)
{
	//Ϊĳ��ѧ����ӿγ�
}

void Account::AccountManager::AddSubjectToTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	//Ϊ��ʦ��ӿγ�
}

void Account::AccountManager::AddClassToTeacher(std::uint64_t teacher_id, std::uint64_t class_name)
{
	//Ϊ��ʦ��Ӱ༶
}

void Account::AccountManager::RemoveSubjectFromStudent(std::uint64_t student_id, std::uint64_t subject_id)
{
	//Ϊѧ��ɾ���γ�
}

void Account::AccountManager::RemoveSubjectFromTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	//Ϊ��ʦɾ���γ�
}

void Account::AccountManager::RemoveClassFromTeacher(std::uint64_t teacher_id, std::uint64_t class_name)
{
	//Ϊ��ʦɾ���༶
}

void Account::AccountManager::DeleteStudent(std::uint64_t student_id)
{
	//��־��Ϊ�������˻�
}

void Account::AccountManager::DeleteTeacher(std::uint64_t teacher_id)
{
	//�����Ϊ�������˻�
}

void Account::ClassesManager::CreateClass(const std::string& class_name, std::uint64_t teacherID, int create_year)
{
	//���༶��Ϣ�洢��Classes.json�ļ���
}

void Account::ClassesManager::AddStudentToClass(std::uint64_t student_id, const std::string class_name)
{
	//���ѧ�����༶��
}

void Account::ClassesManager::RemoveStudentFromClass(const std::string& class_name, std::uint64_t student_id)
{
	//�Ӱ༶���Ƴ�ѧ��
}

void Account::ClassesManager::DeleteClass(const std::string& class_name)
{
	//���༶�в�����ѧ������ɾ���༶���������Ϣ��������ѧ�����׳��쳣
}

void Account::SubjectManager::CreateSubject(std::uint64_t subject_id, const std::string& subject_name, int begin_year, int end_year, const std::string& description)
{
	//�����γ̣�����SubjectsĿ¼�У��Կγ̱����������1000000.json
}

void Account::SubjectManager::AddStudent(std::uint64_t id)
{
	//��γ����ѧ��
}

void Account::SubjectManager::AddTeacher(std::uint64_t id)
{
	//��γ������ʦ
}

void Account::SubjectManager::RemoveStudent(std::uint64_t id)
{
	//�ӿγ��Ƴ�ѧ��
}

void Account::SubjectManager::RemoveTeacher(std::uint64_t id)
{
	//�ӿγ��Ƴ���ʦ
}

void Account::SubjectManager::DeleteSubject(std::uint64_t subject_id)
{
	//ɾ���γ̣�ֻ����ɾ��û����ʦû��ѧ���Ŀγ̣������׳��쳣
}

Account::AccountException::AccountException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Account::AccountException::what(void) const noexcept
{
	return this->reason.c_str();
}

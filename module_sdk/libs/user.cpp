#include "user.h"
#include <shared_mutex>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include <fmt/format.h>
#include "../../rbslib/Storage.h"
#include "../../rbslib/FileIO.h"
#include "../../json/CJsonObject.h"
#include "../../log/logger.h"
constexpr auto STUDENT_DIR = "Students";
constexpr auto CLASSES_FILE = "Classes.json";
constexpr auto TEACHER_DIR = "Teachers";
constexpr auto SUBJECT_DIR = "Subjects";
//��������Ҫͬʱ�������ʱ���մ��ϵ��µ�˳�������ֹ����
std::shared_mutex Global_Student_Mutex;//��дѧ����Ϣ�ļ�ʱ����
std::shared_mutex Global_Teacher_Mutex;//��д��ʦ��Ϣ�ļ�ʱ���� 
std::shared_mutex Global_Classes_Mutex;//��д�༶��Ϣ�ļ�ʱ����
std::shared_mutex Global_Subjects_Mutex;

static inline void CreateDefaultClassesFile(void)
{
	RbsLib::Storage::FileIO::File fp(CLASSES_FILE, RbsLib::Storage::FileIO::OpenMode::Write |
		RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer("{}"));
}

static inline void AddClassToTeacher(std::uint64_t id, const std::string& class_name)
{
	using namespace RbsLib::Storage;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", TEACHER_DIR, id));
	if (file.IsExist() == false) throw Account::AccountException("Teacher not exist");
	auto fp = file.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	if (false == obj.Parse(fp.Read(file.GetFileSize()).ToString()))
		throw Account::AccountException(fmt::format("Parse file {} failed : ", file.Path(), obj.GetErrMsg()));
	if (false == obj["Classes"].Add(class_name))
		throw Account::AccountException(fmt::format("Add class to teacher failed"));
	fp.Close();
	fp = file.Open(FileIO::OpenMode::Write|FileIO::OpenMode::Replace, FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

static inline void RemoveClassFromTeacher(std::uint64_t id, const std::string& class_name)
{
	using namespace RbsLib::Storage;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", TEACHER_DIR, id));
	if (file.IsExist() == false) throw Account::AccountException("Teacher not exist");
	auto fp = file.Open(FileIO::OpenMode::Read, FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	if (false == obj.Parse(fp.Read(file.GetFileSize()).ToString()))
		throw Account::AccountException(fmt::format("Parse file {} failed : ", file.Path(), obj.GetErrMsg()));
	std::string str;
	for (int i = 0; i < obj["Classes"].GetArraySize(); ++i)
	{
		if (obj["Classes"](i) == class_name)
		{
			obj["Classes"].Delete(i);
			break;
		}
	}
	fp.Close();
	fp = file.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

static inline void AddSubjectToStudent(std::uint64_t subject_id, std::uint64_t stu_id)
{
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(".");
	auto fp = file[STUDENT_DIR][std::to_string(stu_id) + ".json"].Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file[STUDENT_DIR][std::to_string(stu_id) + ".json"].GetFileSize()).ToString());
	if (false == obj["Subjects"].Add(subject_id)) throw Account::AccountException("Can not add subject id to student");
	fp.Close();
	fp = file[STUDENT_DIR][std::to_string(stu_id) + ".json"].Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}
static inline void RemoveSubjectFromStudent(std::uint64_t subject_id, std::uint64_t stu_id)
{
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(".");
	auto fp = file[STUDENT_DIR][std::to_string(stu_id) + ".json"].Open(OpenMode::Read | OpenMode::Write, SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file[STUDENT_DIR][std::to_string(stu_id) + ".json"].GetFileSize()).ToString());
	for (int i = 0; i < obj["Subjects"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (obj["Subjects"].Get(i, temp) == false)
			throw Account::AccountException("Get number from subjects failed");
		if (temp == subject_id) 
		{
			obj["Subjects"].Delete(i);
			break;
		}
	}
	fp.Close();
	fp = file[STUDENT_DIR][std::to_string(stu_id) + ".json"].Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

static inline void AddSubjectToTeacher(std::uint64_t subject_id, std::uint64_t teacher_id)
{
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(".");
	auto fp = file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].GetFileSize()).ToString());
	if (false == obj["Subjects"].Add(subject_id)) throw Account::AccountException("Can not add subject id to teacher");
	fp.Close();
	fp = file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

static inline void RemoveSubjectFromTeacher(std::uint64_t subject_id, std::uint64_t teacher_id)
{
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(".");
	auto fp = file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].Open(OpenMode::Read | OpenMode::Write, SeekBase::begin);
	neb::CJsonObject obj(fp.Read(file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].GetFileSize()).ToString());
	for (int i = 0; i < obj["Subjects"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (obj["Subjects"].Get(i, temp) == false)
			throw Account::AccountException("Get number from subjects failed");
		if (temp == subject_id)
		{
			obj["Subjects"].Delete(i);
			break;
		}
	}
	fp.Close();
	fp = file[TEACHER_DIR][std::to_string(teacher_id) + ".json"].Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

void Account::AccountManager::CreateStudent(
	std::uint64_t ID,
	const std::string& name,
	const std::string& phone_number, const std::string & email,
	const std::string& student_sex,
	const std::string& enrollment_date,
	const std::string& pass_word,
	const std::string& college,
	const std::string& class_name,
	const std::string& notes, int permission_level)
{
	//��ѧ����Ϣ����� Students Ŀ¼�£���ѧ����������100000.json
	std::unique_lock<std::shared_mutex> lock(Global_Student_Mutex);
	RbsLib::Storage::StorageFile dir("Students");
	RbsLib::Storage::StorageFile now_dir(".");
	if (dir.IsExist() == false) now_dir.CreateDir("Students");
	if (dir[std::to_string(ID) + ".json"].IsExist()) throw AccountException("Student already exist");
	auto fp = dir[std::to_string(ID) + ".json"].Open(RbsLib::Storage::FileIO::OpenMode::Write |
		RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	obj.Add("Name", name);
	obj.Add("PhoneNumber", phone_number);
	obj.Add("Email", email);
	obj.Add("Sex", student_sex);
	obj.Add("EnrollmentDate", enrollment_date);
	obj.AddEmptySubArray("Subjects");
	obj.Add("College", college);
	obj.Add("Class", class_name);
	obj.Add("Password", pass_word);
	obj.Add("PermissionLevel", permission_level);
	obj.Add("Notes", notes);
	obj.Add("IsEnable", true);
	std::string str = obj.ToFormattedString();
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
	fp.Close();
	std::unique_lock<std::shared_mutex> class_lock(Global_Classes_Mutex);
	neb::CJsonObject class_obj;
	try
	{
		RbsLib::Storage::FileIO::File cfp(CLASSES_FILE, RbsLib::Storage::FileIO::OpenMode::Read, RbsLib::Storage::FileIO::SeekBase::begin);
		auto buffer = cfp.Read(RbsLib::Storage::StorageFile(CLASSES_FILE).GetFileSize());
		cfp.Close();
		if (false == class_obj.Parse(buffer.ToString()))
			throw AccountException("Classes.json parsed failed");
		if (class_obj.KeyExist(class_name) == false)
			throw AccountException("Target class is not exist");
		if (false == class_obj[class_name]["Students"].Add(ID))
			throw AccountException("Add to class students list failed");
		cfp.Open(CLASSES_FILE, RbsLib::Storage::FileIO::OpenMode::Write | RbsLib::Storage::FileIO::OpenMode::Replace,
			RbsLib::Storage::FileIO::SeekBase::begin);
		cfp.Write(RbsLib::Buffer(class_obj.ToFormattedString()));
	}
	catch (const AccountException& ex)
	{
		dir[std::to_string(ID) + ".json"].Remove();
		throw;
	}
}

void Account::AccountManager::CreateTeacher(
	std::uint64_t ID,
	const std::string& name,
	const std::string& phone_number, const std::string & email,
	const std::string& teacher_sex,
	const std::string& college,
	const std::string& pass_word,
	const std::string& notes, int permission_level)
{
	//����ʦ��Ϣ����� Teachers Ŀ¼�£��Թ�����������10000.json
	std::unique_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	RbsLib::Storage::StorageFile dir("Teachers");
	RbsLib::Storage::StorageFile now_dir(".");
	if (dir.IsExist() == false) now_dir.CreateDir("Teachers");
	if (dir[std::to_string(ID) + ".json"].IsExist()) throw AccountException("Teachers already exist");
	auto fp = dir[std::to_string(ID) + ".json"].Open(RbsLib::Storage::FileIO::OpenMode::Write |
		RbsLib::Storage::FileIO::OpenMode::Replace, RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	obj.Add("Name", name);
	obj.Add("Sex", teacher_sex);
	obj.Add("PhoneNumber", phone_number);
	obj.Add("Email", email);
	obj.Add("College", college);
	obj.AddEmptySubArray("Subjects");
	obj.AddEmptySubArray("Classes");
	obj.Add("Password", pass_word);
	obj.Add("PermissionLevel", permission_level);
	obj.Add("Notes", notes);
	obj.Add("IsEnable", true);
	std::string str = obj.ToFormattedString();
	fp.Write(RbsLib::Buffer(obj.ToFormattedString()));
}

bool Account::AccountManager::IsStudentExist(std::uint64_t id)
{
	std::shared_lock<std::shared_mutex> lock(Global_Student_Mutex);
	if (RbsLib::Storage::StorageFile(STUDENT_DIR).IsExist())
		if (RbsLib::Storage::StorageFile(STUDENT_DIR)[std::to_string(id) + ".json"].IsExist())
			return true;
	return false;
}

bool Account::AccountManager::IsTeacherExist(std::uint64_t id)
{
	std::shared_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	if (RbsLib::Storage::StorageFile(TEACHER_DIR).IsExist())
		if (RbsLib::Storage::StorageFile(TEACHER_DIR)[std::to_string(id) + ".json"].IsExist())
			return true;
	return false;
}

void Account::AccountManager::DeleteStudent(std::uint64_t student_id)
{
	//��־��Ϊ�������˻�
}

void Account::AccountManager::DeleteTeacher(std::uint64_t teacher_id)
{
	//�����Ϊ�������˻�
}

auto Account::AccountManager::GetStudentInfo(std::uint64_t id)->StudentBasicInfo
{
	std::shared_lock<std::shared_mutex> lock(Global_Student_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", STUDENT_DIR, id));
	RbsLib::Storage::FileIO::File fp = file.Open
	(RbsLib::Storage::FileIO::OpenMode::Read,
		RbsLib::Storage::FileIO::SeekBase::begin);
	auto buffer = fp.Read(file.GetFileSize());
	neb::CJsonObject obj;
	if (false == obj.Parse(buffer.ToString()))
		throw AccountException("Parse student json text failed");
	StudentBasicInfo info;
	info.id = id;
	info.name = obj("Name");
	info.sex = obj("Sex");
	info.email = obj("Email");
	info.phone_number = obj("PhoneNumber");
	obj.Get("EnrollmentDate", info.enrollment_date);
	info.class_name = obj("Class");
	info.college = obj("College");
	info.password = obj("Password");
	obj.Get("PermissionLevel", info.permission_level);
	info.notes = obj("Notes");
	obj.Get("IsEnable", info.is_enable);
	for (int i = 0; i < obj["Subjects"].GetArraySize(); ++i)
	{
		std::uint64_t temp;
		obj["Subjects"].Get(i, temp);
		info.subjects.push_back(temp);
	}
	return info;
}

auto Account::AccountManager::GetTeacherInfo(std::uint64_t id) -> TeacherBasicInfo
{
	std::shared_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", TEACHER_DIR, id));
	RbsLib::Storage::FileIO::File fp = file.Open
	(RbsLib::Storage::FileIO::OpenMode::Read,
		RbsLib::Storage::FileIO::SeekBase::begin);
	auto buffer = fp.Read(file.GetFileSize());
	neb::CJsonObject obj;
	if (false == obj.Parse(buffer.ToString()))
		throw AccountException("Parse teacher json text failed");
	TeacherBasicInfo info;
	info.id = id;
	info.name = obj("Name");
	info.sex = obj("Sex");
	info.email = obj("Email");
	info.phone_number = obj("PhoneNumber");
	info.college = obj("College");
	info.password = obj("Password");
	obj.Get("PermissionLevel", info.permission_level);
	info.notes = obj("Notes");
	obj.Get("IsEnable", info.is_enable);
	for (int i = 0; i < obj["Subjects"].GetArraySize(); ++i)
	{
		std::uint64_t temp;
		obj["Subjects"].Get(i, temp);
		info.subjects.push_back(temp);
	}
	for (int i = 0; i < obj["Classes"].GetArraySize(); ++i)
	{
		info.classes.push_back(obj["Classes"](i));
	}
	return info;
}

void Account::ClassesManager::CreateClass(const std::string& class_name, std::uint64_t teacherID)
{
	//���༶��Ϣ�洢��Classes.json�ļ���
	std::unique_lock<std::shared_mutex> teacher_lock(Global_Teacher_Mutex);
	std::unique_lock<std::shared_mutex> lock(Global_Classes_Mutex);
	RbsLib::Storage::StorageFile classes_file(CLASSES_FILE);
	if (classes_file.IsExist() == false) CreateDefaultClassesFile();
	auto class_fp = classes_file.Open(RbsLib::Storage::FileIO::OpenMode::Read,
		RbsLib::Storage::FileIO::SeekBase::begin);
	auto buffer = class_fp.Read(classes_file.GetFileSize());
	neb::CJsonObject class_json;
	if (class_json.Parse(buffer.ToString()) == false)
	{
		Logger::LogError("�����ļ�%s����ʧ�ܣ�ԭ��: %s", CLASSES_FILE, class_json.GetErrMsg());
		std::abort();
	}
	//���γ��Ƿ��Ѿ�����
	if (class_json.KeyExist(class_name))
		throw AccountException("Class is already exist");
	//����ʦ�ļ����
	AddClassToTeacher(teacherID,class_name);
	//����γ���Ϣ
	class_fp.Close();
	neb::CJsonObject subjson;
	subjson.Add("TeacherID", teacherID);
	subjson.Add("CreateTime", Time::GetFormattedTime());
	subjson.AddEmptySubArray("Students");
	class_json.Add(class_name, subjson);
	class_fp = classes_file.Open(RbsLib::Storage::FileIO::OpenMode::Write| RbsLib::Storage::FileIO::OpenMode::Replace,
		RbsLib::Storage::FileIO::SeekBase::begin);
	class_fp.Write(RbsLib::Buffer(class_json.ToFormattedString()));
}

void Account::ClassesManager::DeleteClass(const std::string& class_name)
{
	//���༶�в�����ѧ������ɾ���༶���������Ϣ��������ѧ�����׳��쳣
}

bool Account::ClassesManager::IsClassExist(const std::string& class_name)
{
	std::shared_lock<std::shared_mutex> lock(Global_Classes_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(CLASSES_FILE);
	if (file.IsExist())
	{
		auto fp = file.Open(OpenMode::Read, SeekBase::begin);
		if (neb::CJsonObject(fp.Read(file.GetFileSize()).ToString()).KeyExist(class_name))
			return true;
	}
	return false;
}

void Account::SubjectManager::CreateSubject(std::uint64_t subject_id, const std::string& subject_name, int begin_year, int end_year, const std::string & classroom, const std::string& description)
{
	using namespace RbsLib::Storage;
	//�����γ̣�����SubjectsĿ¼�У��Կγ̱����������1000000.json
	std::unique_lock<std::shared_mutex> lock(Global_Subjects_Mutex);
	//���Ŀ¼�Ƿ����
	if (StorageFile(SUBJECT_DIR).IsExist() == false)
		StorageFile(".").CreateDir(SUBJECT_DIR);
	//���γ��Ƿ��Ѿ�����
	StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	if (file.IsExist()) throw AccountException("Subject is already exist");
	neb::CJsonObject object;
	object.Add("SubjectName", subject_name);
	object.Add("Semester", fmt::format("{}-{}", begin_year, end_year));
	object.Add("Description", description);
	object.Add("Classroom", classroom);
	object.AddEmptySubArray("TeacherID");
	object.AddEmptySubArray("StudentsID");
	auto fp = file.Open(FileIO::OpenMode::Write | FileIO::OpenMode::Replace, FileIO::SeekBase::begin);
	fp.Write(RbsLib::Buffer(object.ToFormattedString()));
}

void Account::SubjectManager::AddStudent(std::uint64_t student_id, std::uint64_t subject_id, const std::string & notes)
{
	//��γ����ѧ��
	std::unique_lock<std::shared_mutex> lock(Global_Student_Mutex);
	std::unique_lock<std::shared_mutex> lock2(Global_Subjects_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject json(fp.Read(file.GetFileSize()).ToString());
	neb::CJsonObject sub;
	sub.Add("ID", student_id);
	sub.Add("Grade", -1);
	sub.Add("Notes", notes);
	if (json["StudentsID"].Add(sub) == false)
		throw AccountException("Add student to subject failed");
	AddSubjectToStudent(subject_id, student_id);
	fp.Close();
	fp = file.Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(json.ToFormattedString()));
}

void Account::SubjectManager::AddTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	//��γ������ʦ
	std::unique_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	std::unique_lock<std::shared_mutex> lock2(Global_Subjects_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject json(fp.Read(file.GetFileSize()).ToString());
	if (json["TeacherID"].Add(teacher_id) == false)
		throw AccountException("Add teacher to subject failed");
	AddSubjectToTeacher(subject_id, teacher_id);
	fp.Close();
	fp = file.Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(json.ToFormattedString()));
}

void Account::SubjectManager::RemoveStudent(std::uint64_t student_id, std::uint64_t subject_id)
{
	//�ӿγ��Ƴ�ѧ��
	std::unique_lock<std::shared_mutex> lock(Global_Student_Mutex);
	std::unique_lock<std::shared_mutex> lock2(Global_Subjects_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject json(fp.Read(file.GetFileSize()).ToString());
	for (int i = 0; i < json["StudentsID"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (json["StudentsID"][i].Get("ID", temp) == false)
			throw AccountException("Read students list from subject failed");
		if (temp == subject_id) json["StudentsID"].Delete(i);
	}
	RemoveSubjectFromStudent(subject_id, student_id);
	fp.Close();
	fp = file.Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(json.ToFormattedString()));
}

void Account::SubjectManager::RemoveTeacher(std::uint64_t teacher_id, std::uint64_t subject_id)
{
	//�ӿγ��Ƴ���ʦ
	std::unique_lock<std::shared_mutex> lock(Global_Teacher_Mutex);
	std::unique_lock<std::shared_mutex> lock2(Global_Subjects_Mutex);
	using namespace RbsLib::Storage::FileIO;
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(OpenMode::Read, SeekBase::begin);
	neb::CJsonObject json(fp.Read(file.GetFileSize()).ToString());
	for (int i = 0; i < json["TeacherID"].GetArraySize(); ++i)
	{
		std::uint64_t temp = 0;
		if (json["TeacherID"].Get(i,temp) == false)
			throw AccountException("Read teacher list from subject failed");
		if (temp == subject_id) json["TeacherID"].Delete(i);
	}
	RemoveSubjectFromStudent(subject_id, teacher_id);
	fp.Close();
	fp = file.Open(OpenMode::Write | OpenMode::Replace, SeekBase::begin);
	fp.Write(RbsLib::Buffer(json.ToFormattedString()));
}

void Account::SubjectManager::DeleteSubject(std::uint64_t subject_id)
{
	//ɾ���γ̣�ֻ����ɾ��û����ʦû��ѧ���Ŀγ̣������׳��쳣
}

bool Account::SubjectManager::IsSubjectExist(std::uint64_t subject_id)
{
	std::shared_lock<std::shared_mutex> lock(Global_Subjects_Mutex);
	if (RbsLib::Storage::StorageFile(SUBJECT_DIR).IsExist())
		if (RbsLib::Storage::StorageFile(SUBJECT_DIR)[std::to_string(subject_id) + ".json"].IsExist())
			return true;
	return false;
}

Account::SubjectInfo Account::SubjectManager::GetSubjectInfo(std::uint64_t subject_id)
{
	std::shared_lock<std::shared_mutex> lock(Global_Subjects_Mutex);
	RbsLib::Storage::StorageFile file(fmt::format("{}/{}.json", SUBJECT_DIR, subject_id));
	auto fp = file.Open(RbsLib::Storage::FileIO::OpenMode::Read,
		RbsLib::Storage::FileIO::SeekBase::begin);
	neb::CJsonObject obj;
	if (false == obj.Parse(fp.Read(file.GetFileSize()).ToString()))
		throw AccountException("Can not parse subject info");
	SubjectInfo info;
	info.classroom = obj("Classroom");
	info.description = obj("Description");
	info.id = subject_id;
	info.name = obj("SubjectName");
	std::sscanf(obj("Semester").c_str(), "%d-%d", &info.semester_start, &info.semester_end);
	for (int i = 0; i < obj["TeacherID"].GetArraySize(); ++i)
	{
		std::uint64_t teacher_id = 0;
		obj["TeacherID"].Get(i, teacher_id);
		info.teachers.push_back(teacher_id);
	}
	for (int i = 0; i < obj["StudentsID"].GetArraySize(); ++i)
	{
		SubjectInfo::Student grade;
		obj["StudentsID"][i].Get("ID", grade.id);
		obj["StudentsID"][i].Get("Grade", grade.grade);
		obj["StudentsID"][i].Get("Notes", grade.notes);
		info.students.push_back(grade);
	}
	return info;
}

Account::AccountException::AccountException(const std::string& reason) noexcept
	:reason(reason)
{
}

const char* Account::AccountException::what(void) const noexcept
{
	return this->reason.c_str();
}

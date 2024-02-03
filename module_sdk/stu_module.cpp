#include "module_sdk.h"
#include <iostream>
#include "libs/user.h"

//��������

void Login(const RbsLib::Network::HTTP::Request& request)
{
	std::cout << "login" << std::endl;
	std::cout << request.content.ToString() << std::endl;
	RbsLib::Network::HTTP::ResponseHeader header;
	header.status = 200;
	header.status_descraption = "ok";
	header.headers.AddHeader("Content-Type", "application/json");
	std::string json = "{\"message\":\"ok\",\"token\":\"12345\"}";
	header.headers.AddHeader("Content-Length", std::to_string(json.length()));
	request.connection.Send(header.ToBuffer());
	request.connection.Send(RbsLib::Buffer(json));
}

//��ʼ������������ģ������ĳ�ʼ������Ҫ������ģ�����ư汾��������Ϣ
ModuleSDK::ModuleInfo Init(void)
{
	//����ģ����Ϣ�ṹ�壬���ֱ�����ģ�����ơ��汾������
	ModuleSDK::ModuleInfo info("stu", "1.0.0", "ѧ���ɼ�����ģ��");

	//Ϊģ�����һ������Ϊfunc�ķ�����������func����ʱ��ִ��func1����

	//�����һ������
	info.Add("login", Login);

	//��ģ����Ϣ����
	return info;
}
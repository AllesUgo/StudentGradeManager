#include <stdio.h>
#include <cJSON.h>
#include <iostream>
using namespace std;

void CreatStudentTable()
{
	cJSON* TCP = cJSON_CreateObject();//��������
	cJSON_AddnumberToObject(TCP, "student_id", student_id);//ѧ��id
	cJSON_AddStringToObject(TCP, "name", name);//ѧ������
	cJSON* SUB = cJSON_CreateArray();//����һ������ר�Ŵ洢�γ�
	for (int i = 0; i < 20; i++)
	{
		if (subject[i] != 0)
		{
			cJSON_AddStringToObject(SUB, "", subject[i]);
		}
	}
	cJSON_AddItemToObject(TCP, "subject", SUB);
}
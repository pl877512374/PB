// CubicSplineInterpolation.h: interface for the Interp class.
//
/*/////////////////////////////////////////////////////////////////////
*
*class Interp  ��������ֵ������
*���� *QQ��29720770
*�������������㷨�������һ������ʹ�õ�C++��
*����Ե��X Yֵ���� double ���飬ʹ��ʱ���ԴӴ���̳�һ���࣬��װ�����Լ�������
*
*ʹ�ýӿ� 
* Interp(double *ctrlPtX,double *ctrlPtY,int nCtrlPtCount);  ������Ƶ㣬���캯��
* bool GetInterpolationPts(int outPtCount,double* outPtX,double* outPtY);       ��ò�ֵ��ĵ�����
* bool GetYByX(const double &dbInX, double &dbOutY);                            ��õ���Xֵ����Ӧ��Yֵ��������
*
*//////////////////////////////////////////////////////////////////////
#if !defined(AFX_CUBICSPLINEINTERPOLATION_H__58C0304E_719D_4738_B86C_26BFA529B203__INCLUDED_)
#define AFX_CUBICSPLINEINTERPOLATION_H__58C0304E_719D_4738_B86C_26BFA529B203__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stdio.h>
#include <math.h> 

class Interp  
{
public:

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//
	//ctrlPtX ���Ƶ�X����
	//ctrlPtY ���Ƶ�Y����
	//nCtrlPtCount ���Ƶ���Ŀ�����Ƶ�Ҫ���ڵ���3.
	//
	//////////////////////////////////////////////////////////////////////
	Interp(double *ctrlPtX,double *ctrlPtY,int nCtrlPtCount);
	virtual ~Interp();
public:

	//////////////////////////////////////////////////////////////////////////
	//outPtCount ��Ҫ����Ĳ�ֵ����Ŀ,����ĵ�����Ҫ����1
	//outPtX     �Ѿ�������ڴ��Xֵ�����顣
	//outPtY     �Ѿ�������ڴ��Yֵ�����顣
	//
	//���ô˺�������ò�ֵ������
	//
	//����ɹ�����true������ʧ�ܷ���false
	//////////////////////////////////////////////////////////////////////////
	bool GetInterpolationPts(int outPtCount,double* outPtX,double* outPtY);
	//////////////////////////////////////////////////////////////////////////
	//����Xֵ ����Yֵ
	//dbInX   x�Ա���ֵ������
	//dbOutY  ����õ���Yֵ ���
	//////////////////////////////////////////////////////////////////////////
	bool GetYByX(const double &dbInX, double &dbOutY);
protected:
	void ReleaseMem();
	void InitParam();
	bool InterPolation();
	bool Spline();
	bool ArraySort(int);
protected:
	bool m_bCreate; //���Ƿ񴴽��ɹ��������Ƶ��Ƿ���Ч
	int N;   //������Ƶ�����
	int M;   //����Ĳ��������
	typedef double* pDouble;
	pDouble X,Y; //����Ŀ��Ƶ�����
	pDouble Z,F; //����Ŀ��Ƶ�����
	pDouble H,A,B,C,D; //��࣬���������м�����
};
#endif // !defined(AFX_CUBICSPLINEINTERPOLATION_H__58C0304E_719D_4738_B86C_26BFA529B203__INCLUDED_)

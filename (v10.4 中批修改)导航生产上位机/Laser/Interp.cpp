// CubicSplineInterpolation.cpp: implementation of the Interp class.   
//   
//////////////////////////////////////////////////////////////////////   

#include "stdAfx.h"   
#include "Interp.h"   


//////////////////////////////////////////////////////////////////////   
// Construction/Destruction   
//   
//ctrlPtX ���Ƶ�X����   
//ctrlPtY ���Ƶ�Y����   
//nCtrlPtCount ���Ƶ���Ŀ�����Ƶ�Ҫ���ڵ���3.   
//   
//////////////////////////////////////////////////////////////////////   

Interp::Interp(double *ctrlPtX,double *ctrlPtY,int nCtrlPtCount)   
{   
	InitParam();   // ��ʼ������

	if (NULL == ctrlPtX || NULL == ctrlPtY || nCtrlPtCount < 3 )   
	{   
		m_bCreate = false;   
	}   
	else  
	{   
		N = nCtrlPtCount - 1;   

		int nDataCount = N + 1;   
		X = new double[nDataCount];   //ָ���µ�����
		Y = new double[nDataCount];   

		A = new double[nDataCount];   
		B = new double[nDataCount];   
		C = new double[nDataCount];   
		D = new double[nDataCount];   
		H = new double[nDataCount];   

		memcpy(X,ctrlPtX,nDataCount*sizeof(double));   
		memcpy(Y,ctrlPtY,nDataCount*sizeof(double));   
		m_bCreate = ArraySort(nDataCount);
		if (m_bCreate)
		{
			m_bCreate = Spline();        
		}
		  
	}      
}   

//////////////////////////////////////////////////////////////////////////   
//outPtCount ��Ҫ����Ĳ�ֵ����Ŀ,����ĵ�����Ҫ����1   
//outPtX     �Ѿ�������ڴ��Xֵ�����顣   
//outPtY     �Ѿ�������ڴ��Yֵ�����顣   
//   
//���ô˺�������ò�ֵ������   
//   
//����ɹ�����true������ʧ�ܷ���false   
//////////////////////////////////////////////////////////////////////////   
bool Interp::GetInterpolationPts(int outPtCount, double *outPtX, double *outPtY)   
{   
	if (!m_bCreate)   
	{   
		return m_bCreate;   
	}   

	M = outPtCount - 1;   

	if (M == 0)   
	{   
		return false;   
	}   

	Z = outPtX;   
	F = outPtY;   



	return InterPolation();   


}   

Interp::~Interp()   
{   
	ReleaseMem();   
}   

void Interp::InitParam()   
{   
	X = Y = Z = F = A = B = C = D = H = NULL;   

	N = 0;   
	M = 0;   
}   

void Interp::ReleaseMem()   
{   
	delete [] X;   
	delete [] Y;   
	//  delete [] Z;   
	//  delete [] F;   
	delete [] A;   
	delete [] B;   
	delete [] C;   
	delete [] D;   
	delete [] H;   

	InitParam();   
}   


bool Interp::Spline()   
{   
	int i,P,L;   

	for (i=1;i<=N;i++)   
	{   
		H[i-1]=X[i]-X[i-1];   
	}   

	L=N-1;   
	for(i=1;i<=L;i++)   
	{   
		A[i]=H[i-1]/(H[i-1]+H[i]);   
		B[i]=3*((1-A[i])*(Y[i]-Y[i-1])/H[i-1]+A[i]*(Y[i+1]-Y[i])/H[i]);   
	}   
	A[0]=1;   
	A[N]=0;   
	B[0]=3*(Y[1]-Y[0])/H[0];   
	B[N]=3*(Y[N]-Y[N-1])/H[N-1];   

	for(i=0;i<=N;i++)   
	{   
		D[i]=2;   
	}   

	for(i=0;i<=N;i++)   
	{   
		C[i]=1-A[i];   
	}   

	P=N;   
	for(i=1;i<=P;i++)   
	{   

		if (  fabs(D[i]) <= 0.000001 )                                  
		{   
			return false;   
			//    MessageBox(0,"�޽�","��ʾ,MB_OK);   
			//break;   
		}   
		A[i-1]=A[i-1]/D[i-1];   
		B[i-1]=B[i-1]/D[i-1];   
		D[i]=A[i-1]*(-C[i])+D[i];   
		B[i]=-C[i]*B[i-1]+B[i];   
	}   
	B[P]=B[P]/D[P];   
	for(i=1;i<=P;i++)   
	{   
		B[P-i]=B[P-i]-A[P-i]*B[P-i+1];   
	}   

	return true;   
}   

bool Interp::InterPolation()   
{   

	double dbStep = (X[N] - X[0])/(M);   

	for (int i = 0;i <= M ;++i)   
	{   
		Z[i] = X[0] + dbStep*i;   
	}   

	for(int i=1;i<=M;i++)   //���int���� yjun
	{   
		if (!GetYByX(Z[i],F[i]))   
		{   
			return false;   
		}   

	}   

	F[0] = Y[0];   

	return true;   
}   



bool Interp::GetYByX(const double &dbInX, double &dbOutY)   
{   

	if (!m_bCreate)   
	{   
		return m_bCreate;   //m_bCreateΪfasle == 0����
	}   

	double E,E1,K,K1,H1;   
	int j ;    
	if(dbInX<X[0])   
	{   
		j = 0;   

	}   
	else if (dbInX > X[N])   
	{   
		j = N-1;   
	}   
	else  
	{   
		for (j=1;j<=N;j++)   
		{   
			if(dbInX<=X[j])   
			{   
				j=j-1;   

				break;   
			}   
		}   

	}   

	//////////////////////////////////////////////////////////////////////////   
	E=X[j+1]-dbInX;   
	E1=E*E;   
	K=dbInX-X[j];   
	K1=K*K;   
	H1=H[j]*H[j];   

	dbOutY=(3*E1-2*E1*E/H[j])*Y[j]+(3*K1-2*K1*K/H[j])*Y[j+1];   
	dbOutY=dbOutY+(H[j]*E1-E1*E)*B[j]-(H[j]*K1-K1*K)*B[j+1];   
	dbOutY=dbOutY/H1;   
	
	return true;   
} 
//�������x��������ж�
bool Interp::ArraySort(int size)
{
   	int i;
    double temp;
	if (X[0]<X[size-1])  //����x������������
   	{
		for (i=0;i<size-1;i++)
		{
			if (X[i]<X[i+1])
				;
			else
				return false;
		}		
		return true;
   	}
	else                //�ݼ������     
	{
		int len = size/2;
		for (i =0;i<len;i++)
		{ 
			temp = X[i];
			X[i] = X[size-i-1];
			X[size-i-1] =temp;
			temp = Y[i];
			Y[i] = Y[size-i-1];
			Y[size-i-1] =temp;  	
		}
		for (i =0;i<size-1;i++)
		{
			if (X[i]<X[i+1])       //��������
					;
			else
				return false;     //�ǵ�������
		}
		return true;
	}   
}

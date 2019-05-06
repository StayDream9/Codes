#include<stdio.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#define N 10//��ĸ��� 
#define M0 16 //����  ��һ�����ӹ�ϵ
#define M1 18 ////�ڶ������ӹ�ϵ
#define M2 17 ///���������ӹ�ϵ����
#define max 200//���䷶Χ ���ֵ 
#define min 0// ���䷶Χ ��Сֵ
#define Gmax 50//���������� 
#define K 1 //����ϵ��
#define Defaultlength 50 // 
#define g Defaultlength*Defaultlength //����ϵ��
#define f 10 //��������

#define Alpha 0.5    //��ڵ㶯̬�뾶�İ�����������[0,1] 

struct node{//��ά����� 
	int num;//�ڵ����� 
	float x;
	float y;
};

struct edge{
	int v1;
	int v2;
	float naturelen;
};
struct edge * e;//��
struct node point[N];//N��������� 

 
int degree[N];//����Ķ�
float mass[N];//���������
float force[N];//����i���ܵĺ���
float repulsion[N][N];// �������ܵĳ���
float tension[N][N];// �������ܵ�����
float sumforce;//ϵͳ���� 
int m1;
float stability[N];//�ڵ��ȶ��� 
float dynamicr[N];//ÿһ�����Ӧ�Ķ�̬�뾶 

void randinit(){
	int i;
	for(i=0;i<N;i++){
		point[i].num=i+1;
		point[i].x=(float)rand()/RAND_MAX*(max-min)+min;
		point[i].y=(float)rand()/RAND_MAX*(max-min)+min;
		force[i]=0;
	}
	for(i=0;i<N;i++){
		printf("%d %.2f %.2f\n",point[i].num,point[i].x,point[i].y);
	}
}
void saveinitfile(){//�ѵ������д���ļ� 
	int i;
	FILE *fp;
	fp=fopen("point0.txt","w");
	if(fp==NULL){
		return ;
	}
	for(i=0;i<N;i++){
		fprintf(fp,"%d %f %f\n",point[i].num,point[i].x,point[i].y);
	} 
	fclose(fp);
}
void savefile(){//�ѵ������д���ļ� 
	int i;
	FILE *fp;
	fp=fopen("point1.txt","w");
	if(fp==NULL){
		return ;
	}
	for(i=0;i<N;i++){
		fprintf(fp,"%d %f %f\n",point[i].num,point[i].x,point[i].y);
	} 
	fclose(fp);
}
void readfile(){
	FILE *fp;
	m1=M0;
	e=(struct edge *)malloc(sizeof(struct edge)*m1);
	fp=fopen("edge.txt","r");
	if(fp==NULL){
		return ;
	}
	int i;
	int w,r;
	
	i=0;
	while(fscanf(fp,"%d%d",&w,&r)!=EOF){
		e[i].v1=w;
		e[i].v2=r;
	//	printf("w:%d,r:%d,",w,r);
		e[i].naturelen=Defaultlength;
		i++;
		if(i>=m1)break;
	}
	fclose(fp);
}
void computedegree(){//�������ӹ�ϵ�����������Ķ� 
	int i,j;
	for(i=0;i<N;i++){
		degree[i]=0;
	}
	for(i=0;i<m1;i++){
	//	printf("e:%d,",e[i].v1-1);
		degree[e[i].v1-1]+=1;
		degree[e[i].v2-1]+=1;
	}
	
	for(i=0;i<N;i++){
		mass[i]=degree[i];
	//	printf("%d ,",degree[i]);
	}
	
	return ; 
}

void computetension(){//�������� 
	int i,j;
	float d;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			tension[i][j]=0;
		}
	}
	for(i=0;i<m1;i++){
		d=(point[e[i].v1-1].x-point[e[i].v2-1].x)*(point[e[i].v1-1].x-point[e[i].v2-1].x)+(point[e[i].v1-1].y-point[e[i].v2-1].y)*(point[e[i].v1-1].y-point[e[i].v2-1].y);
		d=sqrt(d);
		if(d<=e[i].naturelen){
			tension[e[i].v1-1][e[i].v2-1]=0;
			tension[e[i].v2-1][e[i].v1-1]=0;
		}
		else{
			tension[e[i].v2-1][e[i].v1-1]=K*(d-e[i].naturelen);
			tension[e[i].v1-1][e[i].v2-1]=K*(d-e[i].naturelen);
		}
	}
} 
void computerepulsion(){//������� 
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			repulsion[i][j]=0;
		}
	}
	float d;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			if(i==j)continue;
			d=(point[i].x-point[j].x)*(point[i].x-point[j].x)+(point[i].y-point[j].y)*(point[i].y-point[j].y);
			if(d==0){
				repulsion[i][j]=f;
			}
			else {
				repulsion[i][j]=g*mass[i]*mass[j]/(d);
			}
		}
	}
}
void main(){
	m1=M0;
	srand(time(NULL));
	randinit();
	
	readfile();//��ȡ���ӹ�ϵ 
	saveinitfile();//���������ʼ���ĵ����� 
	computedegree();//�����ĶȺ����� 
	
	int k=0;
	int i,j; 
	while(k++<Gmax){
		
		computetension();//�������� 
		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				force[i]+=tension[i][j];
					point[i].x+=(point[j].x-point[i].x)*0.001*(tension[i][j]/mass[i]);
					point[i].y+=(point[j].y-point[i].y)*0.001*(tension[i][j]/mass[i]);
			}
		}	
		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				if(i!=j){
					force[i]+=repulsion[i][j];
					point[i].x+=(point[i].x-point[j].x)*0.001*(repulsion[i][j]/mass[i]);
					point[i].y+=(point[i].y-point[j].y)*0.001*(repulsion[i][j]/mass[i]);
				}
			}
		}
		for(i=0;i<N;i++){
			force[i]=0;
		}
		printf("%d����\n",k);
		for(i=0;i<N;i++){
			printf("%d %f %f\n",point[i].num,point[i].x,point[i].y);
		}
	}	
	savefile();
	//FR��� 
	//�µ����ӹ�ϵ�ڶ���
	printf("--------------------------------\n");
		printf("--------------------------------\n");
		printf("--------------------------------\n");
		printf("--------------------------------\n");
		printf("--------------------------------\n");
		printf("�µ����ӹ�ϵ����\n");
		FILE *fp;
		fp=fopen("edge1.txt","r");
		if(fp==NULL){
			printf("�ļ���ʧ��\n");
			return ;
		}
	//	int i;
		int w,r;
		struct edge * temp;//�µ����ӹ�ϵ
		int t;
		
		t=M1;
		temp=(struct edge*)malloc(sizeof(struct edge)*t);
		i=0;
		while(fscanf(fp,"%d%d",&w,&r)!=EOF){
			temp[i].v1=w;
			temp[i].v2=r;
		//	printf("w:%d,r:%d,",w,r);
			temp[i].naturelen=Defaultlength;
			i++;
			if(i>=t)break;
		}
		fclose(fp);
		//����ڵ��ȶ��� 1  0.5   0 
		int d1[N][N]={0};
		int d2[N][N]={0};
		for(i=0;i<m1;i++){
			d1[e[i].v1-1][e[i].v2-1]=d1[e[i].v2-1][e[i].v1-1]=1;
		}
		for(i=0;i<t;i++){
			d2[temp[i].v1-1][temp[i].v2-1]=d2[temp[i].v2-1][temp[i].v1-1]=1;
		}
		int aaa;//���� 
		for(i=0;i<N;i++){
			aaa=0;
			for(j=0;j<N;j++){
				if(d1[i][j]!=d2[i][j])aaa++;
			} 
			if(aaa==0)stability[i]=0;
			else if(aaa==1)stability[i]=0.5;
			else if(aaa>=2)stability[i]=1.0;
		}
		
		//����ڵ㶯̬�뾶 
		for(i=0;i<N;i++)
		{
			dynamicr[i]=Alpha*stability[i];
			degree[i]=0; 
			float ss=0;
			for(j=0;j<N;j++){
				if(d2[i][j]!=0){
					degree[i]++;
					ss+=stability[j];
				}	
			}
			dynamicr[i]+=(1-Alpha)*ss/degree[i];
		}
		//���е���
		for(i=0;i<N;i++){
			mass[i]=degree[i];
		} 
		free(e);
		e=temp;
		m1=t;
		k=0;
		printf("�¶�������ӹ�ϵ��\n");
		for(i=0;i<m1;i++){
			printf("%d %d %.2f\n",e[i].v1,e[i].v2,e[i].naturelen);
		}
		printf("-------------------\n");
		struct node tmppoint[N];//��ʱ�ڵ��� 
		while(k++<Gmax){
			for(i=0;i<N;i++){//δ�ƶ�ǰ�Ľڵ�λ������tmppoint�� 
				tmppoint[i].num=point[i].num;
				tmppoint[i].x=point[i].x;
				tmppoint[i].y=point[i].y;
			} 
			computetension();//�������� 
			for(i=0;i<N;i++){
				for(j=0;j<N;j++){
					force[i]+=tension[i][j];
						point[i].x+=(point[j].x-point[i].x)*0.001*(tension[i][j]/mass[i]);
						point[i].y+=(point[j].y-point[i].y)*0.001*(tension[i][j]/mass[i]);
				}
			}	
			for(i=0;i<N;i++){
				for(j=0;j<N;j++){
					if(i!=j){
						force[i]+=repulsion[i][j];
						point[i].x+=(point[i].x-point[j].x)*0.001*(repulsion[i][j]/mass[i]);
						point[i].y+=(point[i].y-point[j].y)*0.001*(repulsion[i][j]/mass[i]);
					}
				}
			}
			for(i=0;i<N;i++){
				force[i]=0;
			}
			float dd;
			for(i=0;i<N;i++){
				dd=(tmppoint[i].x-point[i].x)*(tmppoint[i].x-point[i].x)+(tmppoint[i].y-point[i].y)*(tmppoint[i].y-point[i].y);
				dd=sqrt(dd);
				if(dd>dynamicr[i]){//����ƶ�������ڶ�̬�뾶 
					point[i].x=tmppoint[i].x+(point[i].x-tmppoint[i].x)*dynamicr[i]/dd;
					point[i].y=tmppoint[i].y+(point[i].y-tmppoint[i].y)*dynamicr[i]/dd;
				}
			}
			printf("%d����\n",k);
			for(i=0;i<N;i++){
				printf("%d %f %f\n",point[i].num,point[i].x,point[i].y);
			}
	
		}
		//����֮�󣬱����µ����� 
	
			fp=fopen("point2.txt","w");
			if(fp==NULL){
				return ;
			}
			for(i=0;i<N;i++){
				fprintf(fp,"%d %f %f\n",point[i].num,point[i].x,point[i].y);
			} 
			fclose(fp);	 
	
		printf("--------------------------------\n");
		printf("--------------------------------\n");
		printf("--------------------------------\n");
		printf("--------------------------------\n");
		printf("--------------------------------\n");
		printf("�µ����ӹ�ϵ����\n");
		fp=fopen("edge2.txt","r");
		if(fp==NULL){
			printf("�ļ���ʧ��\n");
			return;
		}
	
		t=M2;
		temp=(struct edge*)malloc(sizeof(struct edge)*t);
		i=0;
		while(fscanf(fp,"%d%d",&w,&r)!=EOF){
			temp[i].v1=w;
			temp[i].v2=r;
		//	printf("w:%d,r:%d,",w,r);
			temp[i].naturelen=Defaultlength;
			i++;
			if(i>=t)break;
		}
		fclose(fp);
		//����ڵ��ȶ��� 1  0.5   0 
		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				d1[i][j]=0;
				d2[i][j]=0;
			}
		}
		for(i=0;i<m1;i++){
			d1[e[i].v1-1][e[i].v2-1]=d1[e[i].v2-1][e[i].v1-1]=1;
		}
		for(i=0;i<t;i++){
			d2[temp[i].v1-1][temp[i].v2-1]=d2[temp[i].v2-1][temp[i].v1-1]=1;
		}
		//aaa;//���� 
		for(i=0;i<N;i++){
			aaa=0;
			for(j=0;j<N;j++){
				if(d1[i][j]!=d2[i][j])aaa++;
			} 
			if(aaa==0)stability[i]=0;
			else if(aaa==1)stability[i]=0.5;
			else if(aaa>=2)stability[i]=1.0;
		}
		
		//����ڵ㶯̬�뾶 
		for(i=0;i<N;i++)
		{
			dynamicr[i]=Alpha*stability[i];
			degree[i]=0; 
			float ss=0;
			for(j=0;j<N;j++){
				if(d2[i][j]!=0){
					degree[i]++;
					ss+=stability[j];
				}	
			}
			dynamicr[i]+=(1-Alpha)*ss/degree[i];
		}
		//���е���
		for(i=0;i<N;i++){
			mass[i]=degree[i];
		} 
		free(e);
		e=temp;
		m1=t;
		k=0;
		printf("�¶�������ӹ�ϵ��\n");
		for(i=0;i<m1;i++){
			printf("%d %d %.2f\n",e[i].v1,e[i].v2,e[i].naturelen);
		}
		printf("-------------------\n");
		while(k++<Gmax){
			for(i=0;i<N;i++){//δ�ƶ�ǰ�Ľڵ�λ������tmppoint�� 
				tmppoint[i].num=point[i].num;
				tmppoint[i].x=point[i].x;
				tmppoint[i].y=point[i].y;
			} 
			computetension();//�������� 
			for(i=0;i<N;i++){
				for(j=0;j<N;j++){
					force[i]+=tension[i][j];
						point[i].x+=(point[j].x-point[i].x)*0.001*(tension[i][j]/mass[i]);
						point[i].y+=(point[j].y-point[i].y)*0.001*(tension[i][j]/mass[i]);
				}
			}	
			for(i=0;i<N;i++){
				for(j=0;j<N;j++){
					if(i!=j){
						force[i]+=repulsion[i][j];
						point[i].x+=(point[i].x-point[j].x)*0.001*(repulsion[i][j]/mass[i]);
						point[i].y+=(point[i].y-point[j].y)*0.001*(repulsion[i][j]/mass[i]);
					}
				}
			}
			for(i=0;i<N;i++){
				force[i]=0;
			}
			float dd;
			for(i=0;i<N;i++){
				dd=(tmppoint[i].x-point[i].x)*(tmppoint[i].x-point[i].x)+(tmppoint[i].y-point[i].y)*(tmppoint[i].y-point[i].y);
				dd=sqrt(dd);
				if(dd>dynamicr[i]){//����ƶ�������ڶ�̬�뾶 
					point[i].x=tmppoint[i].x+(point[i].x-tmppoint[i].x)*dynamicr[i]/dd;
					point[i].y=tmppoint[i].y+(point[i].y-tmppoint[i].y)*dynamicr[i]/dd;
				}
			}
			printf("%d����\n",k);
			for(i=0;i<N;i++){
				printf("%d %f %f\n",point[i].num,point[i].x,point[i].y);
			}
		
		}
		//����֮�󣬱����µ����� 
		
			fp=fopen("point3.txt","w");
			if(fp==NULL){
				return ;
			}
			for(i=0;i<N;i++){
				fprintf(fp,"%d %f %f\n",point[i].num,point[i].x,point[i].y);
			} 
			fclose(fp);	 
	 
	 
	return ;
}

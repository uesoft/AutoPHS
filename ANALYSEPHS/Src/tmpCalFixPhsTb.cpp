// tmpCalFixPhsTb.cpp: implementation of the tmpCalFixPhsTb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tmpCalFixPhsTb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

tmpCalFixPhsTb::tmpCalFixPhsTb()
{

}

tmpCalFixPhsTb::~tmpCalFixPhsTb()
{

}

BOOL tmpCalFixPhsTb::SetData( _RecordsetPtr pRs, _RecordsetPtr pOtherRs )
{
	CString sTmp = _T( "" );
	rsTmp1.Edit();
				//rsTmp1.SetFieldValue(_T("CustomIDsa"),COleVariant(Ptype(i)));
				rsTmp1.SetFieldValue(_T("CustomIDsa"),COleVariant(sPartID));
				rsTmp1.SetFieldValue(_T("H"),COleVariant(modPHScal::PAfixH));
				rsTmp1.SetFieldValue(_T("gdw2"),COleVariant(modPHScal::gdw));
				rsTmp1.SetFieldValue(_T("tj"),COleVariant(modPHScal::t0));
				//许用应力单位kgf/cm2->kgf/mm2
				rsTmp1.SetFieldValue(_T("SIGMAjt"),COleVariant(Sigmat/100.0));
				rsTmp1.SetFieldValue(_T("SIGMAj20"),COleVariant(Sigma0/100.0));
				rsTmp1.SetFieldValue(_T("SIGMAj100"),COleVariant(Sigmat100/100.0));
				rsTmp1.Update();
				//以上一句加快更新，尽量防止锁定。
				
				//获得构件3
				rsX.GetFieldValue(_T("P3"),vTmp);
				strP3=vtos(vTmp);	
				rsTmp1.Edit();
				rsTmp1.SetFieldValue(_T("P3"),vTmp);
				//此处只适应中国标准，国际化时要修改代码
				n=strP3.Find((_T("x")));
				//构件3腿宽bb,unit:mm
				bb=_tcstod(strP3.Mid(1,n-1),NULL);
				rsTmp1.SetFieldValue(_T("bb"),COleVariant(bb));
				//获得主型钢P1
				rsX.GetFieldValue(_T("P1"),vTmp);
				strP1=vtos(vTmp);
				rsTmp1.SetFieldValue(_T("P1"),vTmp);
				//获得构件2型钢P2
				rsX.GetFieldValue(_T("P2"),vTmp);
				strP2=vtos(vTmp);
				rsTmp1.SetFieldValue(_T("P2"),vTmp);
				//双槽钢外缘最大距离PL3,mm
				rsX.GetFieldValue(_T("PL3"),vTmp);
				PL3=vtof(vTmp);
				//rsX中的PL3是CHAR(20),而rsTmp1中的PL3是REAL,可见变体能够自动转换。
				rsTmp1.SetFieldValue(_T("PL3"),vTmp);
				//槽钢焊缝高度K,mm
				rsX.GetFieldValue(_T("size2"),vTmp);
				K=vtof(vTmp);
				rsTmp1.SetFieldValue(_T("K"),vTmp);
				rsTmp1.Update();
				//序号DH,mm
				rsX.GetFieldValue(_T("dh"),vTmp);
				dh=vtoi(vTmp);				
				
				//先计算热态
				
				bool bEOF=false;
				bool bErr=false;//任意一个条件不满足，置为true
				do
				{
					SSteelPropertyCSTb sSteelTb;

					if(!pOtherRs->FindFirst(_T("BH=\'")+strP1+_T("\'")))
					{
						//在槽钢特性表%s中没找到槽钢型号%s
						return FALSE;
					}
					
					if( !sSteelTb.GetData( pOtherRs ) )
						return FALSE;
					
				
					//构件1槽钢惯性矩Ix,cm4->mm4
					rsTmp1.Edit();
					rsTmp1.SetFieldValue(_T("J1y"),COleVariant(sSteelTb.J1y));
					//构件1槽钢惯性矩Iy,cm4->mm4
					rsTmp1.SetFieldValue(_T("J1z"),COleVariant(sSteelTb.J1z));
					//构件1槽钢最小惯性矩,cm4->mm4
					rsTmp1.SetFieldValue(_T("Imin1"),COleVariant(sSteelTb.J1z>sSteelTb.J1y?sSteelTb.J1y:sSteelTb.J1z));
					//构件1槽钢抗弯矩Wx,cm3->mm3
					rsTmp1.SetFieldValue(_T("W1y"),COleVariant(sSteelTb.W1y));
					//构件1槽钢抗弯矩Wy,cm3->mm3
					rsTmp1.SetFieldValue(_T("W1z"),COleVariant(sSteelTb.W1z));
					//构件1槽钢截面积S,cm2->mm2
					rsTmp1.SetFieldValue(_T("F1"),COleVariant(sSteelTb.F1));
					//构件1槽钢高度h,mm
					rsTmp1.SetFieldValue(_T("h1"),COleVariant(sSteelTb.h1));
					//构件1槽钢腿宽b,mm
					rsTmp1.SetFieldValue(_T("b1"),COleVariant(sSteelTb.b1));
					rsTmp1.Update();
					
					if(!pOtherRs->FindFirst(_T("BH=\'")+strP2+_T("\'")))
					{
						//在槽钢特性表%s中没找到槽钢型号%s
						return FALSE;
					}
					//构件2槽钢惯性矩Ix,cm4->mm4
					rsTmp1.Edit();
					rsTmp1.SetFieldValue(_T("J2y"),COleVariant(J2y));
					//构件2槽钢惯性矩Iy,cm4->mm4
					rsTmp1.SetFieldValue(_T("J2z"),COleVariant(J2z));
					//构件1槽钢最小惯性矩,cm4->mm4
					rsTmp1.SetFieldValue(_T("Imin2"),COleVariant(J2z>J2y?J2y:J2z));
					//构件2槽钢抗弯矩Wx,cm3->mm3
					rsTmp1.SetFieldValue(_T("W2y"),COleVariant(W2y));
					//构件2槽钢抗弯矩Wy,cm3->mm3
					rsTmp1.SetFieldValue(_T("W2z"),COleVariant(W2z));
					//构件2槽钢截面积S,cm2->mm2
					rsTmp1.SetFieldValue(_T("F2"),COleVariant(F2));
					//构件2槽钢高度h,mm
					rsTmp1.SetFieldValue(_T("h2"),COleVariant(h2));
					//构件2槽钢腿宽b,mm
					rsTmp1.SetFieldValue(_T("b2"),COleVariant(b2));
					
					//双槽钢净空距离g,mm
					float g=PL3-2*h1;
					rsTmp1.SetFieldValue(_T("g"),COleVariant(g));
					//双槽钢中心距B1,mm
					float B1=PL3-h1;
					//因为rsTmp1中存在字段b1,而Jet数据库字段名是不区分大小写的，故换B1->Bx,表中相应的公式B1已经替换。
					rsTmp1.SetFieldValue(_T("Bx"),COleVariant(B1));
					//更新数据准备计算
					rsTmp1.Update();					
					
					int i4 = 0;
					float mfpx,mfpy,mfpz;//保存冷或热态力，kgf
					float mfmx,mfmy,mfmz;//保存冷或热态力矩，kgf.m
					//热态/冷态计算
					for(i4=1;i4<=2;i4++)
					{
						if(i4==2)
						{
							mfpx=modPHScal::pxrc;
							mfpy=modPHScal::pyrc;
							mfpz=modPHScal::pzrc;
							mfmx=modPHScal::mxrc;
							mfmy=modPHScal::myrc;
							mfmz=modPHScal::mzrc;
						}
						else
						{
							mfpx=modPHScal::pxlc;
							mfpy=modPHScal::pylc;
							mfpz=modPHScal::pzlc;
							mfmx=modPHScal::mxlc;
							mfmy=modPHScal::mylc;
							mfmz=modPHScal::mzlc;
						}
						
						rsTmp1.Edit();
						rsTmp1.SetFieldValue(_T("Px"),COleVariant(mfpx));
						rsTmp1.SetFieldValue(_T("Py"),COleVariant(mfpy));
						rsTmp1.SetFieldValue(_T("Pz"),COleVariant(mfpz));
						rsTmp1.SetFieldValue(_T("Mx"),COleVariant(mfmx));
						rsTmp1.SetFieldValue(_T("My"),COleVariant(mfmy));
						rsTmp1.SetFieldValue(_T("Mz"),COleVariant(mfmz));
						rsTmp1.Update();
						
						//构件1和构件2焊缝的抗弯矩,mm3
						//float Wyh1=0.233*K*(2*h1+g)*(2*h1+g)+1.4*K*(bb+b1)*(h1+0.5*g);
						//float Wzh1=0.233*K*((b1+bb)*(b1+bb)+b1*b1)+0.35*K*(2*h1+g)*(bb+b1);
						//float Wyh2=(1.4*K*h2*h2*h2+4.2*K*h2*B1*B1)/3/(B1+h2)+0.7*K*b2*2*(B1*B1+h2*h2)/(B1+h2);
						//float Wzh2=K*b2*(0.467*b2+1.4*h2);
						
						if(!rsCal.FindFirst(_T("SEQ=1")))
						{
							//根部%s第%s种条件的计算公式没找到。
							sTmp.Format(IDS_SACalFormulaXNotFound,Ptype[i],_T("1"));
							throw sTmp;
						}
						else
						{
							if(i4==2)
							{//几何数据，只计算一次，加快速度
								//rsCal.GetFieldValue(_T("F1saFormula"),vTmp);
								//F1sa=vtos(vTmp);
								//rsCal.GetFieldValue(_T("F2saFormula"),vTmp);
								//F2sa=vtos(vTmp);
								rsCal.GetFieldValue(_T("Wyh1saFormula"),vTmp);
								Wyh1sa=vtos(vTmp);
								rsCal.GetFieldValue(_T("Wzh1saFormula"),vTmp);
								Wzh1sa=vtos(vTmp);
								rsCal.GetFieldValue(_T("Wyh2saFormula"),vTmp);
								Wyh2sa=vtos(vTmp);
								rsCal.GetFieldValue(_T("Wzh2saFormula"),vTmp);
								Wzh2sa=vtos(vTmp);
								
								//应力判据可先获得，方便以后计算
								rsCal.GetFieldValue(_T("SIGMA1saMaxFormula"),vTmp);
								SIGMA1saMax=vtos(vTmp);
								rsCal.GetFieldValue(_T("TAO1saMaxFormula"),vTmp);
								TAO1saMax=vtos(vTmp);
								rsCal.GetFieldValue(_T("SIGMA2saMaxFormula"),vTmp);
								SIGMA2saMax=vtos(vTmp);
								rsCal.GetFieldValue(_T("TAO2saMaxFormula"),vTmp);
								TAO2saMax=vtos(vTmp);
								rsCal.GetFieldValue(_T("lamda1Formula"),vTmp);
								lamda1=vtos(vTmp);
								rsCal.GetFieldValue(_T("lamda2Formula"),vTmp);
								lamda2=vtos(vTmp);
								
								//开始计算几何、截面特性
								SQLx=_T("SELECT ")+ Wyh1sa+_T(" AS Wyh1sac, ")+ Wyh2sa+_T(" AS Wyh2sac, ") + Wzh1sa+_T(" AS Wzh1sac, ")+ Wzh2sa+_T(" AS Wzh2sac, ")+SIGMA1saMax+_T(" AS SIGMA1saMaxc, ")+TAO1saMax+_T(" AS TAO1saMaxc, ")+SIGMA2saMax+_T(" AS SIGMA2saMaxc, ")+TAO2saMax+_T(" AS TAO2saMaxc, ")+lamda1+_T(" AS lamda1c, ")+lamda2+_T(" AS lamda2c FROM tmpCalFixPhs");
								if(rsTmp2.IsOpen())
									rsTmp2.Close();
								rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
								rsTmp2.Open(dbOpenDynaset,SQLx);
								
								rsTmp1.Edit();
								rsTmp2.GetFieldValue(_T("Wyh1sac"),vTmp);
								rsTmp1.SetFieldValue(_T("Wyh1"),vTmp);
								
								rsTmp2.GetFieldValue(_T("Wzh1sac"),vTmp);
								rsTmp1.SetFieldValue(_T("Wzh1"),vTmp);
								
								rsTmp2.GetFieldValue(_T("Wyh2sac"),vTmp);
								rsTmp1.SetFieldValue(_T("Wyh2"),vTmp);
								
								rsTmp2.GetFieldValue(_T("Wzh2sac"),vTmp);
								rsTmp1.SetFieldValue(_T("Wzh2"),vTmp);							
								
								rsTmp2.GetFieldValue(_T("TAO1saMaxc"),vTmp);
								rsTmp1.SetFieldValue(_T("TAO1saMax"),vTmp);
								mfTAO1saMax=vtof(vTmp);
								rsTmp2.GetFieldValue(_T("SIGMA1saMaxc"),vTmp);
								rsTmp1.SetFieldValue(_T("SIGMA1saMax"),vTmp);
								mfSIGMA1saMax=vtof(vTmp);
								
								rsTmp2.GetFieldValue(_T("TAO2saMaxc"),vTmp);
								rsTmp1.SetFieldValue(_T("TAO2saMax"),vTmp);
								mfTAO2saMax=vtof(vTmp);
								rsTmp2.GetFieldValue(_T("SIGMA2saMaxc"),vTmp);
								rsTmp1.SetFieldValue(_T("SIGMA2saMax"),vTmp);
								mfSIGMA2saMax=vtof(vTmp);
								
								rsTmp2.GetFieldValue(_T("lamda1c"),vTmp);
								rsTmp1.SetFieldValue(_T("lamda1"),vTmp);
								mfLamda1=vtof(vTmp);
								rsTmp2.GetFieldValue(_T("lamda2c"),vTmp);
								rsTmp1.SetFieldValue(_T("lamda2"),vTmp);
								mfLamda2=vtof(vTmp);
								
								rsTmp1.Update();
								float mf1=0.0,mf2=0,mfl1=0,mfl2=0;
								SQLx=_T("SELECT Lamda,Fi FROM SteadyDecreaseCoef WHERE Material=\'") + m_strMaterial + _T("\'");
								if(rs2.IsOpen())
									rs2.Close();
								rs2.m_pDatabase=&EDIBgbl::dbSACal;//20071103 "dbSORT" 改为 "dbSACal"
								rs2.Open(dbOpenSnapshot,SQLx);
								if(rs2.IsEOF() && rs2.IsBOF())
								{
									//在%s库稳定折减系数表%s中没有%s这种材料。
									sTmp.Format(IDS_NoMaterialInSteadyDecreaseCoef,rs2.m_pDatabase->GetName(),_T("SteadyDecreaseCoef"),m_strMaterial);
									throw sTmp;
								}
								else
								{
									if(!rs2.FindFirst(_T("Lamda>")+ftos(mfLamda1)))
									{
										//计算的细长比%s超过%s库%s表中最大值。
										sTmp.Format(IDS_LamdaOverMaxValue,ftos(mfLamda1),modPHScal::dbZDJcrude.GetName(),_T("SteadyDecreaseCoef"));
										throw sTmp;
									}
									else
									{
										rs2.GetFieldValue(_T("Lamda"),vTmp);
										mfl2=vtof(vTmp);
										rs2.GetFieldValue(_T("Fi"),vTmp);
										mf2=vtof(vTmp);
										rs2.MovePrev();
										if(rs2.IsBOF())
										{
											//一般不可能,因为表中数值从0开始
										}
										else
										{
											//插值求Fi1
											rs2.GetFieldValue(_T("Lamda"),vTmp);
											mfl1=vtof(vTmp);
											rs2.GetFieldValue(_T("Fi"),vTmp);
											mf1=vtof(vTmp);
											mfFi1=(mf2-mf1)/(mfl2-mfl1)*(mfLamda1-mfl1)+mf1;
											rsTmp1.Edit();
											rsTmp1.SetFieldValue(_T("Fi1"),COleVariant(mfFi1));																					
											rsTmp1.Update();
										}
									}
									//求构件2稳定折减系数Fi2
									if(!rs2.FindFirst(_T("Lamda>")+ftos(mfLamda2)))
									{
										//计算的细长比%s超过%s库%s表中最大值。
										sTmp.Format(IDS_LamdaOverMaxValue,ftos(mfLamda2),modPHScal::dbZDJcrude.GetName(),_T("SteadyDecreaseCoef"));
										throw sTmp;
									}
									else
									{
										rs2.GetFieldValue(_T("Lamda"),vTmp);
										mfl2=vtof(vTmp);
										rs2.GetFieldValue(_T("Fi"),vTmp);
										mf2=vtof(vTmp);
										rs2.MovePrev();
										if(rs2.IsBOF())
										{
											//一般不可能,因为表中数值从0开始
										}
										else
										{
											//插值求Fi1
											rs2.GetFieldValue(_T("Lamda"),vTmp);
											mfl1=vtof(vTmp);
											rs2.GetFieldValue(_T("Fi"),vTmp);
											mf1=vtof(vTmp);
											mfFi2=(mf2-mf1)/(mfl2-mfl1)*(mfLamda2-mfl1)+mf1;
											rsTmp1.Edit();
											rsTmp1.SetFieldValue(_T("Fi2"),COleVariant(mfFi2));																					
											rsTmp1.Update();
										}
									}
								}
							}
						}
						
						//取Py1=1.5*(Py+/-fabs(1000*mx+pz*modPHScal::PAfixH)/B1),以绝对值较大者作为校核工况。unit,mm
						//开始热态计算
						float Py1r1=1.5*(mfpy+fabs(1000*mfmx+mfpz*modPHScal::PAfixH)/B1);
						float Py1r2=1.5*(mfpy-fabs(1000*mfmx+mfpz*modPHScal::PAfixH)/B1);
						float Py1r=((fabs(Py1r1)>fabs(Py1r2))?Py1r1:Py1r2);
						
						rsTmp1.Edit();
						rsTmp1.SetFieldValue(_T("Py1"),COleVariant(Py1r1));
						rsTmp1.SetFieldValue(_T("Py2"),COleVariant(Py1r2));
						rsTmp1.Update();
						
						if( (mfpx>=0 && 0>=Py1r) || (0>=mfpx && mfpx>=1.732*Py1r))
						{
							i5=1;
						}
						else if( (mfpx>=1.732*Py1r) && (Py1r>0))
						{
							i5=2;
						}
						if( (Py1r>=0 && 0>mfpx) || (1.732*Py1r>=mfpx && mfpx>=0))
						{
							i5=3;
						}
						else if( (mfpx<=1.732*Py1r) && (Py1r<0))
						{
							i5=4;
						}
						
						if(!rsCal.FindFirst(_T("SEQ=")+ltos(i5)))
						{
							//根部%s第%s种条件的计算公式没找到。
							sTmp.Format(IDS_SACalFormulaXNotFound,Ptype[i],_T("1"));
							throw sTmp;
						}
						else
						{
							rsCal.GetFieldValue(_T("SIGMA1saFormula"),vTmp);
							SIGMA1sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOx1saFormula"),vTmp);
							TAOx1sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOy1saFormula"),vTmp);
							TAOy1sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOz1saFormula"),vTmp);
							TAOz1sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAO1saFormula"),vTmp);
							TAO1sa=vtos(vTmp);
							
							rsCal.GetFieldValue(_T("SIGMA2saFormula"),vTmp);
							SIGMA2sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOx2saFormula"),vTmp);
							TAOx2sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOy2saFormula"),vTmp);
							TAOy2sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAOz2saFormula"),vTmp);
							TAOz2sa=vtos(vTmp);
							rsCal.GetFieldValue(_T("TAO2saFormula"),vTmp);
							TAO2sa=vtos(vTmp);
							
							//压杆的长度系数u1,u2:悬臂=2,简支=0.5							
							rsCal.GetFieldValue(_T("u1"),vTmp);
							rsTmp1.Edit();
							rsTmp1.SetFieldValue(_T("u1"),vTmp);
							rsCal.GetFieldValue(_T("u2"),vTmp);
							rsTmp1.SetFieldValue(_T("u2"),vTmp);
							rsTmp1.Update();
							
							//开始计算正应力sigma、剪应力tao
							SQLx=_T("SELECT ")+ TAOx1sa+_T(" AS TAOx1sac, ")+ TAOy1sa+_T(" AS TAOy1sac, ")+ TAOz1sa+_T(" AS TAOz1sac,")
								+ TAOx2sa+_T(" AS TAOx2sac, ")+ TAOy2sa+_T(" AS TAOy2sac, ")+ TAOz2sa+_T(" AS TAOz2sac FROM tmpCalFixPhs");
							if(rsTmp2.IsOpen())
								rsTmp2.Close();
							rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
							rsTmp2.Open(dbOpenDynaset,SQLx);
							
							rsTmp1.Edit();
							rsTmp2.GetFieldValue(_T("TAOx1sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOx1sa"),vTmp);
							rsTmp2.GetFieldValue(_T("TAOy1sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOy1sa"),vTmp);
							rsTmp2.GetFieldValue(_T("TAOz1sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOz1sa"),vTmp);
							
							rsTmp2.GetFieldValue(_T("TAOx2sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOx2sa"),vTmp);
							rsTmp2.GetFieldValue(_T("TAOy2sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOy2sa"),vTmp);
							rsTmp2.GetFieldValue(_T("TAOz2sac"),vTmp);
							rsTmp1.SetFieldValue(_T("TAOz2sa"),vTmp);
							rsTmp1.Update();
							
							SQLx=_T("SELECT ")+ TAO1sa+_T(" AS TAO1sac,")+ TAO2sa+_T(" AS TAO2sac,")+SIGMA1sa+_T(" AS SIGMA1sac,")+SIGMA2sa+_T(" AS SIGMA2sac FROM tmpCalFixPhs");
							if(rsTmp2.IsOpen())
								rsTmp2.Close();
							rsTmp2.m_pDatabase=&EDIBgbl::dbPRJ;
							rsTmp2.Open(dbOpenDynaset,SQLx);
							
							rsTmp2.GetFieldValue(_T("SIGMA1sac"),vTmp);
							mfSIGMA1sa=vtof(vTmp);
							rsTmp1.Edit();
							rsTmp1.SetFieldValue(_T("SIGMA1sa"),vTmp);
							
							rsTmp2.GetFieldValue(_T("SIGMA2sac"),vTmp);
							mfSIGMA2sa=vtof(vTmp);
							rsTmp1.SetFieldValue(_T("SIGMA2sa"),vTmp);
							
							rsTmp2.GetFieldValue(_T("TAO1sac"),vTmp);
							mfTAO1sa=vtof(vTmp);							
							rsTmp1.SetFieldValue(_T("TAO1sa"),vTmp);
							
							rsTmp2.GetFieldValue(_T("TAO2sac"),vTmp);
							mfTAO2sa=vtof(vTmp);
							rsTmp1.SetFieldValue(_T("TAO2sa"),vTmp);
							rsTmp1.Update();
							
							//构件1(主型钢P1)强度校核(槽钢部分)
							//unit,kgf/mm2
							if(mfSIGMA1sa>mfSIGMA1saMax)
							{
								if(bEOF)
								{
									//%s号构件%s槽钢%s热态正应力值%s超过允许值%s
									sTmp.Format(GetResStr(IDS_SSteelStressOver),ltos(modPHScal::zdjh),_T("1"),strP1,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(mfSIGMA1sa),ftos(mfSIGMA1saMax));
									throw sTmp;
								}
								else
									bErr=true;							
							}
							//构件1(主型钢P1)强度校核(焊缝部分)
							//构件1焊缝合成应力
							if(mfTAO1sa>mfTAO1saMax)
							{
								if(bEOF)
								{
									//%s号构件%s槽钢%s热态正应力值%s超过允许值%s
									sTmp.Format(GetResStr(IDS_SSteelStressOver1),ltos(modPHScal::zdjh),_T("1"),strP1,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(mfTAO1sa),ftos(mfTAO1saMax));
									throw sTmp;
								}
								else
									bErr=true;							
							}
							
							//构件2(斜撑槽钢P2)强度校核(槽钢部分)
							//unit,kgf/mm2
							if(mfSIGMA2sa>mfSIGMA2saMax)
							{
								if(bEOF)
								{
									//%s号构件%s槽钢%s热态正应力值%s超过允许值%s
									sTmp.Format(GetResStr(IDS_SSteelStressOver),ltos(modPHScal::zdjh),_T("2"),strP2,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(mfSIGMA2sa),ftos(mfSIGMA2saMax));
									throw sTmp;
								}
								else
									bErr=true;							
							}
							//构件2(斜撑槽钢P2)强度校核(焊缝部分)
							//构件2焊缝合成应力
							if(mfTAO1sa>mfTAO1saMax)
							{
								if(bEOF)
								{
									//%s号构件%s槽钢%s热态正应力值%s超过允许值%s
									sTmp.Format(GetResStr(IDS_SSteelStressOver1),ltos(modPHScal::zdjh),_T("2"),strP2,GetResStr(IDS_frmtxsr_OptHeatStatus),ftos(mfTAO2sa),ftos(mfTAO2saMax));
									throw sTmp;
								}
								else
									bErr=true;							
							}
						}						
					}
					
					if(!bErr /*&& (dh>=36)*/) //注释掉的部分可测试华东院计算手册例题
					{
						//没超限，找到
						break;
					}
					else
					{
						rsX.MoveNext();
						if(rsX.IsEOF())
						{
							//已经到记录尾，重新循环一次，以便报警
							bEOF=true;
							continue;
						}
						else
							//重新开始之前，设置超限标志
							bErr=false;
						
						//获得构件3
						rsX.GetFieldValue(_T("P3"),vTmp);
						strP3=vtos(vTmp);	
						rsTmp1.Edit();
						rsTmp1.SetFieldValue(_T("P3"),vTmp);
						//此处只适应中国标准，国际化时要修改代码
						n=strP3.Find((_T("x")));
						//构件3腿宽bb,unit:mm
						bb=_tcstod(strP3.Mid(1,n-1),NULL);
						rsTmp1.SetFieldValue(_T("bb"),COleVariant(bb));
						//获得主型钢P1
						rsX.GetFieldValue(_T("P1"),vTmp);
						strP1=vtos(vTmp);
						rsTmp1.SetFieldValue(_T("P1"),vTmp);
						//获得构件2型钢P2
						rsX.GetFieldValue(_T("P2"),vTmp);
						strP2=vtos(vTmp);
						rsTmp1.SetFieldValue(_T("P2"),vTmp);
						//双槽钢外缘最大距离PL3,mm
						rsX.GetFieldValue(_T("PL3"),vTmp);
						PL3=vtof(vTmp);
						//rsX中的PL3是CHAR(20),而rsTmp1中的PL3是REAL,可见变体能够自动转换。
						rsTmp1.SetFieldValue(_T("PL3"),vTmp);
						//槽钢焊缝高度K,mm
						rsX.GetFieldValue(_T("size2"),vTmp);
						K=vtof(vTmp);
						rsTmp1.SetFieldValue(_T("K"),vTmp);
						rsTmp1.Update();
						//序号DH,mm
						rsX.GetFieldValue(_T("dh"),vTmp);
						dh=vtoi(vTmp);				
					}
					
				}while(true);//记录尾再循环一次,以便报警
	return TRUE;
}

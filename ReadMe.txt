bool EDIBgbl::ChangeDatabase(_ConnectionPtr &dDb, _ConnectionPtr &sDb)
{
    bool bIsChange=false;
	try
	{
		CDaoTableDef sTD(&sDb) , dTD(&dDb) ;
		CDaoTableDefInfo sTDInfo,dTDInfo;
		bool bFDexists =false, bTDexists =false;
		FieldPtr sFD , dFD ;
		DAOFields * pF;
		DAOTableDefs * tdfs;
		sDb.m_pDAODatabase ->get_TableDefs (&tdfs);
		tdfs->Refresh();
		tdfs->Release();
		int sc=sDb.GetTableDefCount();

		dDb.m_pDAODatabase ->get_TableDefs (&tdfs);
		tdfs->Refresh();
		tdfs->Release();
		int dc=dDb.GetTableDefCount();

		for ( int si=0;si<sc;si++)
		{	
			sDb.GetTableDefInfo(si,sTDInfo);
			if((sTDInfo.m_lAttributes & dbSystemObject) == 0)
			{
				bTDexists=false;
				sTDInfo.m_strName=_T("[")+sTDInfo.m_strName+_T("]");
				sTD->Open((_bstr_t)(sTDInfo.m_strName);
				for ( int di=0;di<dc;di++)
				{	
					dDb.GetTableDefInfo(di,dTDInfo);
					dTDInfo.m_strName=_T("[")+dTDInfo.m_strName+_T("]");
					sTDInfo.m_strName.MakeUpper();
					dTDInfo.m_strName.MakeUpper();
					if ( ((sTDInfo.m_lAttributes & dbSystemObject) == 0) && (sTDInfo.m_strName== dTDInfo.m_strName)  )
					{
						//表存在
						bTDexists=true;
						sTD.m_pDAOTableDef->get_Fields(&pF);
						pF->Refresh();
						pF->Release();
						int sc1=sTD->RecordCount;
						dTD->Open((_bstr_t)(dTDInfo.m_strName);
						for (int i1=0;i1<sc1;i1++)
						{
							DAO_CHECK(dTD.m_pDAOTableDef->get_Fields(&pF));
							pF->Refresh();
							pF->Release();
							int dc1=dTD->RecordCount;
							sTD.GetFieldInfo(i1,sFD,AFX_DAO_ALL_INFO);
							bFDexists = false;
							for (int n1=0;n1<dc1;n1++)
							{
								dTD.GetFieldInfo(n1,dFD);
								if ( dFD.m_strName == sFD.m_strName ) 
								{
									//字段存在，设置标志
									bFDexists = true;
									break;
								}
							}
							if ( ! bFDexists )
							{
								//不存在这个字段，则修改项目库中的表结构
								//dTD.Fields.Append dTD.CreateField(sFD.Name, sFD.Type, sFD.Size)
								try
								{
									dTD.CreateField( sFD);
									bIsChange=true;
								}
								catch(CException *e)
								{
									e->Delete();
								}
								//i = i + 1
								i++;
							}
						}
						
						dTD->Close();
						break;
					}
				}
				if(!bTDexists)
				{
					SQLx.Format(_T("SELECT * INTO %s FROM %s IN \'%s\'"),sTDInfo.m_strName,sTDInfo.m_strName,sDb->DefaultDatabase);
					dDb->Execute((_bstr_t)SQLx);
				}
			}
			sTD->Close();
		}
	}
	catch(CException *e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	return bIsChange;
}

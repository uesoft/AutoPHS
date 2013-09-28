
extern "C" __declspec(dllexport) int WINAPI  PSAPointCoordinateWirteFile(CString FileNameIn,CString FileNameOut);
int PSAPointCoordinateWirteFile1(CString FileNameIn,CString FileNameOut);
class NodeHeightData
	{
	public :
        NodeHeightData(){Heightx = 0;Heighty = 0;Heightz = 0;}
		~NodeHeightData(){}
		int NodeFrom;
		int NodeTo;
		int NodeType;
		float Heightx;
		float Heighty;
		float Heightz;
	};
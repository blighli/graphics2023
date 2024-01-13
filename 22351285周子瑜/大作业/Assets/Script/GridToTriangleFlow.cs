using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Threading;
using System;
//using VRTK.Examples;
using UnityEngine.UI;

public class GridToTriangleFlow : MonoBehaviour
{
	#region 定义变量

	private Mesh mesh;
	//泥石流网格大小
	int height = 319;
	int width = 301;

    //泥石流起始点坐标
    public static float xllcorner = 359976f;
    public static float yllcorner = 3475000f;

	const int nodata_trn = -9999;
	float[,] gridDepth = null;   //每个网格泥深值
	public static float[,] demDepth = null;//每个网格地形值

    //存储泥深不为0的行列号
    ArrayList xllList,yllList;

	ArrayList TriangleList;//三角形列表索引值
	ArrayList VectorList;//三角列表坐标

    ArrayList DepthList;//泥深值
	public int[] m_triangles;
	public Vector3[] m_vertices;

	#endregion

	int num = 1;
	string number;
    private GameObject planeObj;
	// Use this for initialization
	void Start ()
	{
        demDepth = new float[height, width];
//		//Debug.Log("最开始时间：" + System.DateTime.Now);
        ReadDem20_First();
        for (; num <= 20; num++)
        {
            //初始化
            gridDepth = new float[height, width];
            TriangleList = new ArrayList();
            VectorList = new ArrayList();
            DepthList = new ArrayList();

            number = num.ToString();
            getFiles(number);//得到泥深

            createTriangle();//构建三角网
            getVector();
            outPut(number);
            transformArryList();//转换Triangle和ArrayList中的值
        }
        //      planeObj = GameObject.Find("PlaneFirst");
        planeObj = GameObject.Find("Plane");
		mesh = new Mesh ();
		GetComponent<MeshFilter> ().mesh = mesh;
		mesh.vertices = m_vertices;
	}    
 
	int index = 1;
	float time;
    public static float evolutionTime = 0;
	// Update is called once per frame
	void Update ()
	{
		time += Time.deltaTime;

        MeshFilter thisMeshFilter = planeObj.GetComponent<MeshFilter>();
        Mesh thisMesh = thisMeshFilter.mesh;//MeshFliter里面的mesh

		if ((time >= 0.6f)) {
			if (index <= 20) {

                evolutionTime = index*6;
                
				Mesh mesh = GetComponent<MeshFilter> ().mesh;
				mesh.Clear ();//清空所有顶点和三角形索引
				string number = index.ToString ();
				read (number);

				mesh.vertices = m_vertices;//坐标
				mesh.triangles = m_triangles;//构建点数
                Debug.Log (index + ":" + m_triangles.Length);

                GetComponent<MeshCollider>().sharedMesh = thisMesh;
                GetComponent<MeshCollider>().convex = true;
                GetComponent<MeshCollider>().isTrigger = true;

				index++;
				time = 0;
			}
		}
	}

	#region 读dem20

	void ReadDem20_First ()
	{
		TextAsset dem20 = (TextAsset)Resources.Load<TextAsset> ("terrainClip/dem20_extract1");
		StringReader readerdem = new StringReader (dem20.text);
		if (readerdem == null) {
			Debug.Log ("文件不存在!");
		} else {
			int count = 0;//记录行高数 
			string linedem;
			while ((linedem = readerdem.ReadLine ()) != null) {
				string[] values = linedem.Trim ().Split (new char[] { ' ' });
				if (values.Length != width) {
					continue;
				} else {
					for (int i = 0; i < width; i++) {
						float v = float.Parse (values [i]);
						demDepth [count, i] = v;

					}
					count++;
				}
			}
            readerdem.Close ();
		}
	}

	#endregion

	#region//得到在网格里的索引值

	/// <summary>
	/// Gets the index in grid.
	/// </summary>
	/// <returns>The index in grid.</returns>
	/// <param name="grid">Grid.</param>
	/// <param name="i">The index.</param>
	/// <param name="j">J.</param>
	static int getIndexInGrid (float[,] grid, int i, int j)
	{
		int statistics = 0;
		for (int h = 0; h < grid.GetLength (0); h++) {
			for (int w = 0; w < grid.GetLength (1); w++) {
				if (h == i && w == j) {
					return statistics;
				} else {
					if (grid [h, w] != 0) {
						statistics++;
					}
				}
			}
		}
		return 0;
	}

	#endregion

	#region 读取泥石流文件信息

	void getFiles (string number)
	{
		TextAsset demdata = (TextAsset)Resources.Load<TextAsset> ("flowClip/" + number);
//		TextAsset demdata = (TextAsset)Resources.Load<TextAsset> ("store1/" + number);
		StringReader reader = new StringReader (demdata.text);
		if (reader == null) {
			Debug.Log ("文件不存在");
		} else {
			string line;
			int hang = 0;
			while ((line = reader.ReadLine ()) != null) {
				string[] values = line.Trim ().Split (new char[] { ' ' });
				if (values.Length != width) {
					continue;
				} else {
					for (int i = 0; i < width; i++) {
						float v = float.Parse (values [i]);
						if (Mathf.Approximately (v, nodata_trn)) {
							v = 0.0f;
						}
						gridDepth [hang, i] = v;
					}
					hang++;
				}
			}
            //Debug.Log(number + "文件存在" + gridDepth[123, 34]);
        }
		reader.Close ();
	}

	#endregion

	#region 构建三角形

	void createTriangle ()
	{
		int index1, index2, index3;
		for (int i = 0; i < height - 1; i++) {
			for (int j = 0; j < width - 1; j++) {
				if (gridDepth [i, j] == 0) {
					continue;
				} else {
					if (j > 0) {
						if (gridDepth [i, j - 1] == 0 && gridDepth [i + 1, j - 1] != 0 && gridDepth [i + 1, j] != 0) {
							index3 = getIndexInGrid (gridDepth, i + 1, j - 1);
							index2 = getIndexInGrid (gridDepth, i + 1, j);
							index1 = getIndexInGrid (gridDepth, i, j);

							TriangleList.Add (index3);
							TriangleList.Add (index2);
							TriangleList.Add (index1);
						}
					}
					if (gridDepth [i + 1, j + 1] == 0 && gridDepth [i + 1, j] != 0 && gridDepth [i, j + 1] != 0) {
						index3 = getIndexInGrid (gridDepth, i + 1, j);
						index2 = getIndexInGrid (gridDepth, i, j + 1);
						index1 = getIndexInGrid (gridDepth, i, j);

						TriangleList.Add (index3);
						TriangleList.Add (index2);
						TriangleList.Add (index1);
					}
					if (gridDepth [i + 1, j] != 0 && gridDepth [i + 1, j + 1] != 0) {
						index3 = getIndexInGrid (gridDepth, i + 1, j);
						index2 = getIndexInGrid (gridDepth, i + 1, j + 1);
						index1 = getIndexInGrid (gridDepth, i, j);

						TriangleList.Add (index3);
						TriangleList.Add (index2);
						TriangleList.Add (index1);
					}
					if (gridDepth [i, j + 1] != 0 && gridDepth [i + 1, j + 1] != 0) {
						index3 = getIndexInGrid (gridDepth, i + 1, j + 1);
						index2 = getIndexInGrid (gridDepth, i, j + 1);
						index1 = getIndexInGrid (gridDepth, i, j);

						TriangleList.Add (index3);
						TriangleList.Add (index2);
						TriangleList.Add (index1);
					}
				}
			}
		}
	}

	#endregion

	#region 得到顶点坐标

	void getVector ()
	{
		for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				if (gridDepth [h, w] == 0) {
					continue;
				} else {
					float x = h * 20;
					float z = w * 20;
					float y = gridDepth [h, w] + demDepth [h, w];
					Vector3 pVector = new Vector3 (x, y, z);
					VectorList.Add (pVector);
				}
			}
		}

	}

	#endregion

	#region 转换TriangleList和VectorList里的值

	void transformArryList ()
	{
		m_triangles = new int[TriangleList.Count];
		m_vertices = new Vector3[VectorList.Count];
		for (int i = 0; i < TriangleList.Count; i++) {
			m_triangles [i] = (int)TriangleList [i];
		}

		for (int i = 0; i < VectorList.Count; i++) {
			m_vertices [i] = (Vector3)VectorList [i];
		}
		//Debug.Log (m_vertices [0]);
	}

	#endregion

	#region 输出结果

	void outPut (string num)
	{
		string path = @"D:\unity5.3.5\myproject\nishiliu\" + num + ".txt";
		FileStream fs = new FileStream (path, FileMode.OpenOrCreate, FileAccess.Write);
		StreamWriter sw = new StreamWriter (fs);
		for (int i = 0; i < TriangleList.Count; i++) {
			sw.Write (TriangleList [i] + ";");
        }
		sw.Write ("\r\n");

		for (int i = 0; i < VectorList.Count; i++) {
			sw.Write (VectorList [i] + ";");
		}
		sw.Write ("\r\n");
        sw.Close ();
	}



	#endregion

	#region 再次读取文件 得到m_triangles、m_vertices

	void read (string number1)
	{
		//TextAsset demdata = (TextAsset)Resources.Load<TextAsset> ("store1/" + number1);//原先未裁剪泥石流文件输出的格网值
        TextAsset demdata = (TextAsset)Resources.Load<TextAsset>("debris_flow/" + number1);//裁剪泥石流文件输出的格网值
		StringReader reader = new StringReader (demdata.text);

		int hang = 0; //记录文件行数
		string line;
		while ((line = reader.ReadLine ()) != null) {
			hang++;
			if (hang == 1) {
				string[] values = line.Trim ().Split (new char[] { ';' });
				m_triangles = new int[values.Length - 1];
				for (int i = 0; i < values.Length - 1; i++) {
					m_triangles [i] = int.Parse (values [i]);
				}

			} else if (hang == 2) {
				ArrayList arry = new ArrayList ();
				string[] values = line.Trim ().Split (new char[] { ';' });
				m_vertices = new Vector3[values.Length - 1];
				for (int i = 0; i < values.Length - 1; i++) {
					string[] a = values [i].Trim ().Split ('(');
					string[] b = a [1].Trim ().Split (')');
					string[] c = b [0].Trim ().Split (',');
					float x = float.Parse (c [0]);
					float y = float.Parse (c [1]);
					float z = float.Parse (c [2]);
					m_vertices [i] = new Vector3 (x, y, z);
				}

			}
		}

	}

	#endregion

}

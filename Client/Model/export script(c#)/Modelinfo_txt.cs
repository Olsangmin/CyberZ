using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Windows;
using System.IO;
using UnityEngine.UIElements;

public class Modelinfo_txt : MonoBehaviour
{

    private List<string> m_rTextureNamesListForCounting = new List<string>();
    private List<string> m_rTextureNamesListForWriting = new List<string>();

    private StreamWriter m_rTextWriter = null;
    private int m_nFrames = 0;

    const float EPSILON = 1.0e-6f;

    bool FindTextureByName(List<string> textureNamesList, Texture texture)
    {
        if (texture)
        {
            string strTextureName = string.Copy(texture.name).Replace(" ", "_");
            for (int i = 0; i < textureNamesList.Count; i++)
            {
                if (textureNamesList.Contains(strTextureName)) return (true);
            }
            textureNamesList.Add(strTextureName);
            return (false);
        }
        else
        {
            return (true);
        }
    }

    void WriteTabs(int nLevel)
    {
        for (int i = 0; i < nLevel; i++) m_rTextWriter.Write("\t");
    }

    void WriteObjectName(int nLevel, string strHeader, Object obj)
    {
        WriteTabs(nLevel);
        m_rTextWriter.Write(strHeader + " ");
        m_rTextWriter.WriteLine((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
    }


    void WriteString(int nLevel, string strToWrite)
    {
        WriteTabs(nLevel);
        m_rTextWriter.Write(strToWrite);
    }

    void WriteLineString(string strToWrite)
    {
        m_rTextWriter.WriteLine(strToWrite);
    }

    void WriteLineString(int nLevel, string strToWrite)
    {
        WriteTabs(nLevel);
        m_rTextWriter.WriteLine(strToWrite);
    }

    void WriteVector(Vector3 v)
    {
        m_rTextWriter.Write(v.x + " " + v.y + " " + v.z + " ");
    }

    void WriteVector(Quaternion q)
    {
        m_rTextWriter.Write(q.x + " " + q.y + " " + q.z + " " + q.w + " ");
    }


    void WriteVectors(int nLevel, string strHeader, Vector3[] vectors)
    {
        WriteString(nLevel, strHeader + " " + vectors.Length + " ");
        if (vectors.Length > 0)
        {
            foreach (Vector3 v in vectors) m_rTextWriter.Write(v.x + " " + v.y + " " + v.z + " ");
        }
        m_rTextWriter.WriteLine(" ");
    }


    void WriteIntegers(int nLevel, string strHeader, int[] integers)
    {
        WriteString(nLevel, strHeader + " " + integers.Length + " ");
        if (integers.Length > 0)
        {
            foreach (int i in integers) m_rTextWriter.Write(i + " ");
        }
        m_rTextWriter.WriteLine(" ");
    }

    void WriteMatrix(Matrix4x4 matrix)
    {
        m_rTextWriter.Write(matrix.m00 + " " + matrix.m10 + " " + matrix.m20 + " " + matrix.m30 + " ");
        m_rTextWriter.Write(matrix.m01 + " " + matrix.m11 + " " + matrix.m21 + " " + matrix.m31 + " ");
        m_rTextWriter.Write(matrix.m02 + " " + matrix.m12 + " " + matrix.m22 + " " + matrix.m32 + " ");
        m_rTextWriter.Write(matrix.m03 + " " + matrix.m13 + " " + matrix.m23 + " " + matrix.m33 + " ");
    }


    void WriteTransform(int nLevel, string strHeader, Transform current)
    {
        WriteString(nLevel, strHeader + " ");
        WriteVector(current.localPosition);
        WriteVector(current.localEulerAngles);
        WriteVector(current.localScale);
        WriteVector(current.localRotation);
        m_rTextWriter.WriteLine(" ");
    }

    void WriteLocalMatrix(int nLevel, string strHeader, Transform current)
    {
        WriteString(nLevel, strHeader + " ");
        Matrix4x4 matrix = Matrix4x4.identity;
        matrix.SetTRS(current.localPosition, current.localRotation, current.localScale);
        WriteMatrix(matrix);
        m_rTextWriter.WriteLine(" ");
    }


    int GetTexturesCount(Material[] materials)
    {
        int nTextures = 0;
        for (int i = 0; i < materials.Length; i++)
        {
            if (materials[i].HasProperty("_MainTex"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_MainTex"))) nTextures++;
            }
            if (materials[i].HasProperty("_SpecGlossMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_SpecGlossMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_MetallicGlossMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_MetallicGlossMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_BumpMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_BumpMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_EmissionMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_EmissionMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_DetailAlbedoMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_DetailAlbedoMap"))) nTextures++;
            }
            if (materials[i].HasProperty("_DetailNormalMap"))
            {
                if (!FindTextureByName(m_rTextureNamesListForCounting, materials[i].GetTexture("_DetailNormalMap"))) nTextures++;
            }
        }
        return (nTextures);
    }

    int GetTexturesCount(Transform current)
    {
        int nTextures = 0;
        MeshRenderer meshRenderer = current.gameObject.GetComponent<MeshRenderer>();
        if (meshRenderer) nTextures = GetTexturesCount(meshRenderer.materials);

        for (int k = 0; k < current.childCount; k++) nTextures += GetTexturesCount(current.GetChild(k));

        return (nTextures);
    }

    void WriteMeshInfo(int nLevel, Mesh mesh)
    {

        WriteLineString(nLevel+1, "<Bounds>: " + mesh.bounds.center.x + " " + mesh.bounds.center.y + " " + mesh.bounds.center.z + " " + mesh.bounds.extents.x + " " + mesh.bounds.extents.y + " " + mesh.bounds.extents.z);

        WriteVectors(nLevel + 1, "<Positions>:", mesh.vertices);
       
     
        WriteLineString(nLevel, "</Mesh>");
    }

    void WriteFrameInfo(int nLevel, Transform current)
    {
        int nTextures = GetTexturesCount(current);
        WriteObjectName(nLevel, "<Frame>: " + m_nFrames++ + " " + nTextures, current.gameObject);

        WriteTransform(nLevel + 1, "<Transform>:", current);
        WriteLocalMatrix(nLevel + 1, "<TransformMatrix>:", current);

        BoxCollider modelBoxCollder = current.gameObject.GetComponent<BoxCollider>();

        if (modelBoxCollder)
        {
            WriteLineString(nLevel, "<Bounds>: " + modelBoxCollder.bounds.center.x + " " + modelBoxCollder.bounds.center.y + " " + modelBoxCollder.bounds.center.z + " " + modelBoxCollder.bounds.extents.x + " " + modelBoxCollder.bounds.extents.y + " " + modelBoxCollder.bounds.extents.z);
        }

    }
    void WriteFrameHierarchyInfo(int nLevel, Transform current)
    {
        WriteFrameInfo(nLevel, current);

        WriteLineString(nLevel + 1, "<Children>: " + current.childCount);
        if (current.childCount > 0)
        {
            for (int k = 0; k < current.childCount; k++) WriteFrameHierarchyInfo(nLevel + 2, current.GetChild(k));
        }
        WriteLineString(nLevel, "</Frame>");
    }
    void WriteFrameNameHierarchy(Transform current)
    {
        m_rTextWriter.Write(string.Copy(current.gameObject.name).Replace(" ", "_") + " ");
        if (current.childCount > 0)
        {
            for (int k = 0; k < current.childCount; k++) WriteFrameNameHierarchy(current.GetChild(k));
        }
    }
    void Start()
    {
    
        m_rTextWriter = new StreamWriter(string.Copy(gameObject.name).Replace(" ", "_") + "_info"+ ".txt");

        WriteLineString("<Hierarchy>:");
        WriteFrameHierarchyInfo(1, transform);
        WriteLineString("</Hierarchy>");


        m_rTextWriter.Flush();
        m_rTextWriter.Close();

        print("Model Text Write Completed");
    }
}

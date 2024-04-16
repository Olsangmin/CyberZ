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
        //WriteTabs(nLevel);
        m_rTextWriter.Write(strHeader + " ");
        m_rTextWriter.WriteLine((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
    }

    void WriteLineString(int nLevel, string strToWrite)
    {
       // WriteTabs(nLevel);
        m_rTextWriter.WriteLine(strToWrite);
    }

    void WriteVector(Vector3 v)
    {
        m_rTextWriter.Write(v.x + " " + v.y + " " + v.z + " ");
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

    void WriteFrameInfo(int nLevel, Transform current)
    {
    
        BoxCollider modelBoxCollder = current.gameObject.GetComponent<BoxCollider>();

        if (modelBoxCollder)
        {
            int nTextures = GetTexturesCount(current);
            WriteObjectName(nLevel, "<Frame>: " + m_nFrames++, current.gameObject);
            WriteLineString(nLevel, "<Bounds>: " + modelBoxCollder.bounds.center.x + " " + modelBoxCollder.bounds.center.y + " " + modelBoxCollder.bounds.center.z + " " + modelBoxCollder.bounds.extents.x + " " + modelBoxCollder.bounds.extents.y + " " + modelBoxCollder.bounds.extents.z);
        }

    }
    void WriteFrameHierarchyInfo(int nLevel, Transform current)
    {
        WriteFrameInfo(nLevel, current);

        if (current.childCount > 0)
        {
            for (int k = 0; k < current.childCount; k++) WriteFrameHierarchyInfo(nLevel + 2, current.GetChild(k));
        }
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
        
        WriteFrameHierarchyInfo(1, transform);

        m_rTextWriter.Flush();
        m_rTextWriter.Close();

        print("Model Text Write Completed");
    }
}

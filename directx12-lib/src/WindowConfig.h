#pragma once

/// <summary>
/// windowの設定
/// </summary>
struct WindowConfig{
	const TCHAR* appName;	//アプリケーションの名前
	UINT width;				//windowウサイズ:幅
	UINT height;			//windowサイズ:高さ
	UINT x;					//windowの位置:x軸
	UINT y;					//windowの位置:y軸
};
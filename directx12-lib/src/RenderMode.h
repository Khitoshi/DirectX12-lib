#pragma once

//描画モード
enum class RenderMode
{
    Solid,          //ソリッドモード
    WireFrame,      //ワイヤーフレームモード

    Num,			//モードの数
    Default = Solid,//デフォルトはソリッドモード
};
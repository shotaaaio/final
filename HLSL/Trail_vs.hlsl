//#include "Trail.hlsli"
//VS_OUT main(VS_INPUT input)
//{
//    VS_OUT output;

//    //ワールド空間位置（世界行列を Identity として想定）
//    float3 worldPos = input.Position;

//    //ノイズに応じて、法線方向へオフセット
//    float noise = sin(worldPos.y * NoiseAmount + Time);
//    worldPos += normalize(input.Normal) * noise * DistortionPower;

//    //トレイルの幅を調整
//    worldPos.x += input.TexCoord.x * TrailWidth;

//    //ビュー／プロジェクション変換
//    float4 viewPos = mul(float4(worldPos, 1.0f), View);
//    output.Pos = mul(viewPos, Projection);

//    //色相オフセットを適用
//    float hueShift = HueOffset * input.TexCoord.y;
//    float3 shiftedColor = HSVtoRGB(hueShift, 1.0f, 1.0f); // HSVからRGBに変換
//    output.Color = float4(shiftedColor * input.Color.rgb, input.Color.a);

//    //UV座標はそのまま通す
//    output.TexCoord = input.TexCoord;

//    //法線と WorldPos を渡しておく（必要なら PS で利用可能）
//    output.Normal = input.Normal;
//    output.WorldPos = worldPos;

//    return output;
//}
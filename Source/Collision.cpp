#include"Collision.h"
#include"Graphics/SkinnedMesh.h"

	/// <summary>
	/// 球と球の交差判定
	/// </summary>
	/// <param name="positionA">球A</param>
	/// <param name="radiusA">半径A</param>
	/// <param name="positionB">球B</param>
	/// <param name="radiusB">半径B</param>
	/// <param name="outVec">衝突方向のベクトル（出力用）</param>
	/// <returns>true...衝突した</returns>
bool Collision::intersectSphereAndSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outVec
)
{
	DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&positionB);
	//二点間の位置ベクトルを引く
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(posB, posA);

	//単一の長さを計算
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);

	//float型に変換
	float len;
	DirectX::XMStoreFloat(&len, length);

	//距離判定
	float range = radiusA + radiusB;
	if (len > range)return false;

	//衝突していたら衝突方向のベクトルを設定
	vec = DirectX::XMVector3Normalize(vec);
	DirectX::XMStoreFloat3(&outVec, vec);

	return true;
}

/// <summary>
/// 円柱と円柱の交差判定
/// </summary>
/// <param name="positionA">位置A</param>
/// <param name="radiusA">半径A</param>
/// <param name="heightA">高さA</param>
/// <param name="positionB">位置B</param>
/// <param name="radiusB">半径B</param>
/// <param name="heightB">高さB</param>
/// <param name="outVec">衝突方向のベクトル（出力用）</param>
/// <returns>true...衝突した</returns>

bool Collision::intersectCylinderAndCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outVec)
{
	//Aの足元がBの頭より上なら衝突していない
	if (positionA.y > positionB.y + heightB) return false;

	//Aの頭がBの足元より下なら衝突していない
	if (positionA.y + heightA < positionB.y) return false;

	//XZ平面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range) return false;

	//衝突方向のベクトル
	outVec.x = vx / distXZ;
	outVec.y = 0.0f;
	outVec.z = vz / distXZ;
	return true;
}

bool Collision::intersectSphereAndCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outVec)
{
	//A(球)の足元がBの(円柱)の頭より上なら衝突してない
	if (positionA.y - radiusA > positionB.y + heightB)return false;

	//A（球）の頭が B（円柱）の足もとより下なら衝突していない
	if (positionA.y + radiusA < positionB.y)return false;

	//XZ平面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)return false;

	//衝突方向のベクトル
	outVec.x = vx / distXZ;
	outVec.y = 0.0f;
	outVec.z = vz / distXZ;

	return true;
}

	/// <summary>
	/// レイとモデルの交差判定
	/// </summary>
	/// <param name="start">レイの始点</param>
	/// <param name="end">レイの終点</param>
	/// <param name="model">モデル</param>
	/// <param name="result">ヒット結果</param>
	/// <returns>true...衝突した</returns>
bool Collision::intersectRayAndModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const SkinnedMesh* model,
	HitResult& result)
{
	//ワールド空間上でのレイの視点
	DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
	//ワールド空間上でのレイの終点
	DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
	//ワールド空間上でのレイの始点から終点までのベクトル
	DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
	//ワールド空間上でのレイの長さ
	DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
	DirectX::XMStoreFloat(&result.distance, worldRayLength);

	
	//true...衝突した
	bool hit = false;

	for (const SkinnedMesh::mesh&mesh : model->meshes)
	{
		//レイをワールド空間からローカル空間へ変換
		DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh.default_global_transform);
		DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);

		//ローカル空間上でのレイの始点
		DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
		//ローカル空間上でのレイの終点
		DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
		//ローカル空間上でのレイの始点から終点までのベクトル
		DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
		//ローカル空間上でのレイの方向ベクトル
		DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
		//ローカル空間上での長さ
		DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
		float localRayLength;
		DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);


		//頂点データを取得
		const std::vector<SkinnedMesh::vertex>& vertices = mesh.vertices;
		const std::vector<UINT>& indices = mesh.indices;

		//候補となる情報
		int materialIndex = -1;
		DirectX::XMVECTOR hitPosition;
		DirectX::XMVECTOR hitNormal;

		for (const SkinnedMesh::mesh::subset& subset : mesh.subsets)
		{
			for (UINT i = 0;i < subset.index_count; i+=3)
			{
				UINT index = subset.start_index_location + i;

				//三角形の頂点の抽出
				const SkinnedMesh::vertex& a = vertices.at(indices.at(index));
				const SkinnedMesh::vertex& b = vertices.at(indices.at(index + 1));
				const SkinnedMesh::vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR aVec = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR bVec = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR cVec = DirectX::XMLoadFloat3(&c.position);

				DirectX::XMVECTOR abVec = DirectX::XMVectorSubtract(bVec, aVec);
				DirectX::XMVECTOR bcVec = DirectX::XMVectorSubtract(cVec, bVec);
				DirectX::XMVECTOR caVec = DirectX::XMVectorSubtract(aVec, cVec);

				//三角形の法線を計算
				DirectX::XMVECTOR normalVec = DirectX::XMVector3Cross(abVec, bcVec);

				//レイの方向と面の方向をチェック（平面の表裏判定）
				DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(localRayDirectVec, normalVec);
				float dot;
				DirectX::XMStoreFloat(&dot, dotVec);
				if (dot >= 0)continue;

				//レイの始点から交点までの長さを計算
				DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(aVec, localRayStartVec);
				float length = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, normalVec)) / dot;

				//長さが０以下なら以降の処理は飛ばす
				if (length < 0.0f) continue;

				//レイと平面の交点を計算
				DirectX::XMVECTOR hitPosVec = DirectX::XMVectorAdd(localRayStartVec, DirectX::XMVectorScale(localRayDirectVec, length));

				//交点が三角形の内側にあるかどうか判定

				//交点から点Aまでのベクトル
				DirectX::XMVECTOR paVac = DirectX::XMVectorSubtract(aVec, hitPosVec);
				//交点から点Aまでのベクトルと点A点Bのベクトルの外積
				DirectX::XMVECTOR crossVec1 = DirectX::XMVector3Cross(paVac, abVec);
				//外積と法線の向きをチェック
				DirectX::XMVECTOR dotVec1 = DirectX::XMVector3Dot(crossVec1, normalVec);
				float dot1;
				DirectX::XMStoreFloat(&dot1, dotVec1);
				if (dot1 < 0.0f)continue;

				
				//交点から点Bまでのベクトル
				DirectX::XMVECTOR pbVec = DirectX::XMVectorSubtract(bVec, hitPosVec);
				//交点から点Bまでのベクトルと点B点Cのベクトルの外積
				DirectX::XMVECTOR crossVec2 = DirectX::XMVector3Cross(pbVec, bcVec);
				//外積と法線の向きをチェック
				DirectX::XMVECTOR dotVec2 = DirectX::XMVector3Dot(crossVec2, normalVec);
				float dot2;
				DirectX::XMStoreFloat(&dot2, dotVec2);
				if (dot2 < 0.0f)continue;

				
				//交点から点Cまでのベクトル
				DirectX::XMVECTOR pcVec = DirectX::XMVectorSubtract(cVec, hitPosVec);
				//交点から点Cまでのベクトルと点C点Aのベクトルの外積
				DirectX::XMVECTOR crossVec3 = DirectX::XMVector3Cross(pcVec, caVec);
				//外積と法線の向きをチェック
				DirectX::XMVECTOR dotVec3 = DirectX::XMVector3Dot(crossVec3, normalVec);
				float dot3;
				DirectX::XMStoreFloat(&dot3, dotVec3);
				if (dot3 < 0.0f)continue;


				//長さが０以下なら以降の処理は飛ばす
				if (length < 0.0f)continue;

				//交点までの距離が今までに計算した最近距離より大きい時はスキップ
				if (length > localRayLength) continue;


				//最近距離を更新
				localRayLength = length;

				//マテリアル番号を更新
				materialIndex = static_cast<int>(subset.material_unique_id);

				//交点と法線を更新
				hitPosition = hitPosVec;
				hitNormal = normalVec;
			}
		}

		if (materialIndex >= 0)
		{
			//交点座標をローカル空間からワールド空間へ変換
			DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
			//ワールド空間上でのレイの始点から交点までのベクトル
			DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
			//ワールド空間上でのレイの始点から交点までの長さ
			DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
			float distance;
			DirectX::XMStoreFloat(&distance, worldLengthVec);

			//ヒット結果情報保存
			if (result.distance > distance)
			{
				//ヒット時の面を法線をローカル空間からワールド空間へ変換
				DirectX::XMVECTOR worldNormal = DirectX::XMVector3Transform(hitNormal, worldTransformMat);

				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, worldPositionVec);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
				hit = true;
			}
		}
	}

	return hit;
}

bool Collision::squareVsSquare(
	DirectX::XMFLOAT2 posA,
	DirectX::XMFLOAT2 sizeA,
	DirectX::XMFLOAT2 posB,
	DirectX::XMFLOAT2 sizeB
)
{

	return true;
}

bool Collision::intersectSphereCastAndModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	float radius,
	const SkinnedMesh* model,
	HitResult& result)
{
	// スフィアキャストの開始位置と終了位置を取得
	DirectX::XMVECTOR sphereStartVec = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR sphereEndVec = DirectX::XMLoadFloat3(&end);
	float radiusSq = radius * radius;

	bool hit = false;
	float closestDistanceSq = FLT_MAX;

	// モデルのメッシュをループ
	for (const SkinnedMesh::mesh& mesh : model->meshes)
	{
		const std::vector<SkinnedMesh::vertex>& vertices = mesh.vertices;
		const std::vector<UINT>& indices = mesh.indices;

		// メッシュの全ての三角形をチェック
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			const SkinnedMesh::vertex& a = vertices[indices[i]];
			const SkinnedMesh::vertex& b = vertices[indices[i + 1]];
			const SkinnedMesh::vertex& c = vertices[indices[i + 2]];

			// スフィアキャストと三角形の交差判定
			DirectX::XMFLOAT3 intersectionPoint;
			if (intersectSphereAndTriangle(start, end, radius, a, b, c, intersectionPoint))
			{
				// 最も近い交差点を更新
				DirectX::XMVECTOR vecToHit = DirectX::XMVectorSubtract(
					DirectX::XMLoadFloat3(&intersectionPoint),
					sphereStartVec);
				float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vecToHit));

				if (distSq < closestDistanceSq)
				{
					closestDistanceSq = distSq;
					result.position = intersectionPoint;
					hit = true;
				}
			}
		}
	}

	return hit;
}

bool Collision::intersectSphereAndTriangle(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	float radius,
	const SkinnedMesh::vertex& a,
	const SkinnedMesh::vertex& b,
	const SkinnedMesh::vertex& c,
	DirectX::XMFLOAT3& outVec)
{
	using namespace DirectX;

	// スフィアのキャストされた線分を計算
	XMVECTOR A = XMLoadFloat3(&a.position);
	XMVECTOR B = XMLoadFloat3(&b.position);
	XMVECTOR C = XMLoadFloat3(&c.position);
	XMVECTOR sphereStart = XMLoadFloat3(&start);
	XMVECTOR sphereEnd = XMLoadFloat3(&end);

	// 三角形の法線ベクトルを計算
	XMVECTOR AB = XMVectorSubtract(B, A);
	XMVECTOR AC = XMVectorSubtract(C, A);
	XMVECTOR normal = XMVector3Normalize(XMVector3Cross(AB, AC));

	// スフィアキャストの方向ベクトル（start から end への方向）
	XMVECTOR dir = XMVectorSubtract(sphereEnd, sphereStart);

	// 三角形の平面との交差判定
	float dist = XMVectorGetX(XMVector3Dot(XMVectorSubtract(sphereStart, A), normal));

	// 平面との距離がスフィアの半径より大きい場合は交差なし
	if (fabs(dist) > radius) return false;

	// 線分と平面の交点を計算
	float t = -dist / XMVectorGetX(XMVector3Dot(dir, normal));
	if (t < 0.0f || t > 1.0f) return false;  // 線分上での交点がない

	// 線分と平面の交点の位置を計算
	XMVECTOR intersection = XMVectorAdd(sphereStart, XMVectorScale(dir, t));

	// 投影点が三角形内にあるか確認
	XMVECTOR v0 = XMVectorSubtract(C, A);
	XMVECTOR v1 = XMVectorSubtract(B, A);
	XMVECTOR v2 = XMVectorSubtract(intersection, A);

	// 三角形内のバリュ法で判定
	float d00 = XMVectorGetX(XMVector3Dot(v0, v0));
	float d01 = XMVectorGetX(XMVector3Dot(v0, v1));
	float d11 = XMVectorGetX(XMVector3Dot(v1, v1));
	float d20 = XMVectorGetX(XMVector3Dot(v2, v0));
	float d21 = XMVectorGetX(XMVector3Dot(v2, v1));

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0) return false;

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	// 三角形内であれば交差とみなす
	if (u >= 0 && v >= 0 && w >= 0)
	{
		XMStoreFloat3(&outVec, intersection);
		return true;
	}

	return false;
}
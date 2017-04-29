# FoamCAD

流体解析ソフトOpenFOAMの付属ユーティリティBlockMesh用のCADソフト  
▼FoamCADメインウインドウ  
![screenshot1](https://github.com/hal1437/FoamCAD/blob/master/Resource/screenshot/screenshot1.png)
数値流体力学のツールボックスとして知られるOpenFOAMの『メッシュ生成』を行うソフトウェア。
本来は各頂点と辺の定義はテキストで行わなければならないが、
このソフトは視覚的に頂点の配置と辺の定義を行うことができる。

## 機能

+ 視覚的にエッジ(点、直線、円弧、スプライン)を配置できる。
+ 四つの直線、円弧、スプラインを選択することで立体を定義できる。
+ 定義した物体の分割の寄せを表示できる。
+ 各点、直線間に寸法を定義し、寸法に基づいた変形ができる。
+ 点の直接座標を指定できる。
+ BlockMesh用の設定ファイルBlockMeshDictを出力できる。
+ ファイルへの出力

## 実装予定

+ Z軸座標操作
+ 初期条件定義、ソルバー選択
+ BlockMeshDict以外のファイルへの対応
+ BlockMeshやsnappyHexMeshに依存しないメッシュ作成
+ 寸法定義のルールをしっかり守る

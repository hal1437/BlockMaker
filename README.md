# FoamCAD

流体解析ソフトOpenFOAMの付属ユーティリティBlockMesh用のCADソフト  
▼FoamCADメインウインドウ  
![screenshot1](https://github.com/hal1437/FoamCAD/blob/master/Resource/screenshot/screenshot1.png)
数値流体力学のツールボックスとして知られるOpenFOAMの『メッシュ生成』を行うソフトウェア。
本来は各頂点と辺の定義はテキストで行わなければならないが、
このソフトは視覚的に頂点の配置と辺の定義を行うことができる。

## 機能

+ 視覚的に点、直線、円弧、矩形、スプラインを配置できる。
+ 四つの直線、円弧、スプラインを選択することで立体を定義できる。
+ 各点、直線間に寸法を定義し、寸法に基づいた変形ができる。
+ 直接座標を指定できる。
+ BlockMesh用の設定ファイルBlockMeshDictを出力できる。

## 実装予定

+ BlockMeshDict以外のファイルへの対応
+ BlockMeshやsnappyHexMeshに依存しないメッシュ作成
+ 寸法定義のルールをしっかり守る
+ OpenFOAMの内包

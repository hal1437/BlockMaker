# FoamCAD

流体解析ソフトOpenFOAMの付属ユーティリティBlockMesh用のCADソフト  
*三次元操作への対応を進行中...*  


▼FoamCADメインウインドウ  
![screenshot1](https://raw.githubusercontent.com/hal1437/FoamCAD/4cc28c43a0ad2fd0097e316e84a26cdf66ba7d83/Resource/screenshot/screenshot2.png)
数値流体力学のツールボックスとして知られるOpenFOAMの『メッシュ生成』を行うソフトウェア。
本来は各頂点と辺の定義はテキストで行わなければならないが、
このソフトは視覚的に頂点の配置と辺の定義を行うことができる。

## 機能

+ 視覚的にエッジ(点、直線、円弧、スプライン)を配置できる。
+ 三次元上にオブジェクトを配置できる。
+ 四つの直線、円弧、スプラインを選択することで立体を定義できる。
+ 定義した物体の分割の寄せを表示できる。
+ 点の直接座標を指定できる。
+ BlockMesh用の設定ファイルBlockMeshDictを出力できる。
+ 点、エッジ、立体を保存できる独自ファイルへの出力

## 実装予定

+　STLファイルの読み込み
+ 幾何拘束の追加
+ BlockMeshDict以外のファイルへの対応

# openYuanrong 文档

## 文档构建

在 `docs` 目录下执行如下命令安装依赖。

```bash
pip install -r requirements_dev.txt
```

在 `yuanrong` 仓执行如下命令构建 API 用于自动生成 API 文档。

```bash
bash build.sh
```

在 `yuanrong/docs` 目录下执行如下命令构建文档。

```bash
bash build.sh
```

生成的文件在 `yuanrong/output/docs` 目录下。重新构建请先删除 `yuanrong/docs/_build` 目录避免历史构建文件的影响。

## 在本地浏览器中查看文档

在构建结果 `output` 目录下，执行如下命令：

```bash
# <port> 替换为一个可用对外端口
python3 -m http.server <port> -d docs
```

在浏览器访问：`http://<主机 IP>:<port>/index.html`。

## 部署

编译产物中生成的 `yuanrong/output/docs/sitemap.xml` 文件，用于引导搜索引擎爬虫抓取，并加速收录。部署时需替换[sitemap.xml](https://atomgit.com/src-openeuler/yuanrong/blob/docs/sitemap.xml){target="_blank"}文件，并删除原文件。

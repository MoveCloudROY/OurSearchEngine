<div align="center">
  <h1>OurSearchEngine</code></h1>

  <p>
    <strong>A simple search engine written in <code>Cpp</code></strong>
  </p>
</div>

## Features
- Infrastructure based on **inverted indexes**.
- Parallelised Index Creation Process.
- Using **SkipList and FST** to improve the query performance of the project.
- Github CI support.


## Architecture
![Architecture](doc/pic/Arch.png)


## How to use
### Build
```bash
cd OurSearchEngine
git submodule update --init --recursive
cmake -B build
cmake --build build
```

### Use
You could refer to the documents in `test` directory to use this library.



## Reference
1. [Lucene](https://www.infoq.cn/article/ejeg02vroegvalw4j_ll)

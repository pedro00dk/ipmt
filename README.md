# IPMT - Indexed Pattern Matching Tool

IPMT é uma ferramenta para indexação, armazenagem e busca de padrões num arquivo texto. O objetivo da ferramenta é encontrar ocorrências exatas de padrões num texto em tempo linear (na soma dos tamanhos dos padroes) com auxílio de um índice completo previamente computado.

## Algoritmos disponíveis

### Compressão
* LZ-77
* LZ-78

### Indexação
* Árvore de Sufixos
* Array de Sufixos

## Pré-requisitos

Para executar a ferramente, é necessário que a máquina possua o compilador de C++11.

## Setup

1. Baixe o projeto
2. Execute o Makefile para compilar o executável do projeto
3. Configure a variável de ambiente para apontar para o executável

## Opções de linha de comando

```
ipmt index [<option>] textfile ->  generates index file
ipmt search [<option>] pattern indexfile  ->  search indexfile
ipmt <option> <-h | --help>  ->  show global help
ipmt <-h | --help>  -> show option help

Options:
-h,--help                  Display this information
-v,--verbose               Print program runtime information
-e,--compression <arg>     Specify the compression algorithm
-i,--indextype <arg>       Specify the indexing algorithm
-c,--count                 Display only the number of found occurrences
-p,--pattern <arg>         Specify a file with patterns to search
                           If specified, the first global argument (pattern) is treated as a text file

--count and --pattern options should not be specified in index mode


-e,--compression <arg>  Specify the compression algorithm
  arg options: lz77, lz78, uncompressed
-i,--indextype <arg>    Specify the indexing algorithm
  arg options: suffixtree, suffixarray
```

## Exemplo

```
ipmt index moby-dick.txt
ipmt search moby moby-dick.txt.idx
```

## Mais informações
Informações adicionais de implementação e observações estão disponíveis no relatório, na pasta doc.

## Autores

* **Giovanni Barros** - [gaabs](https://github.com/gaabs)
* **Pedro Henrique** - [pedro00dk](https://github.com/pedro00dk)

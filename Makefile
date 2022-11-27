# (1)コンパイラ
CC  = gcc
# (2)コンパイルオプション
CFLAGS    = -Wall 
CFLAGS		+= --coverage -fstack-protector
# (3)実行ファイル名
TARGET  = token-list
# (4)コンパイル対象のソースコード
SRCS    = token-list.c
SRCS		+= scan.c
SRCS		+= parse.c
# (5)オブジェクトファイル名
OBJS    = $(SRCS:.c=.o)
 
# (6)インクルードファイルのあるディレクトリパス
INCDIR  = -I../
 
# (7)ライブラリファイルのあるディレクトリパス
LIBDIR  = 
 
# (8)追加するライブラリファイル
LIBS    = 

LFLAGS	= -lgcov --coverage

# (9)ターゲットファイル生成
$(TARGET): $(OBJS)
	$(CC) -o $@ $(LFLAGS) $^ $(LIBDIR) $(LIBS)
	
# (10)オブジェクトファイル生成
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)

# (11)"make all"で make cleanとmakeを同時に実施。
all: clean $(OBJS) $(TARGET)
# (12).oファイル、実行ファイル、.dファイルを削除
clean:
	-rm -f $(OBJS) $(TARGET) *.d
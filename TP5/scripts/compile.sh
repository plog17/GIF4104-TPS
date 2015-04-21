export HADOOP_PREFIX="/Users/plauger/Development/hadoop-2.5.2"

export HADOOP_CLASSPATH=$JAVA_HOME/lib/tools.jar

$HADOOP_PREFIX/bin/hadoop com.sun.tools.javac.Main *.java
jar cf wc.jar *.class

export HADOOP_CLASSPATH=$JAVA_HOME/lib/tools.jar

$HADOOP_PREFIX/bin/hadoop com.sun.tools.javac.Main *.java
jar cf wc.jar *.class

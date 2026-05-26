# Data Streams

openYuanrong provides data streams based on a publish-subscribe (pub/sub) model, enabling data exchange of unbounded data streams between functions and supporting complex data interaction relationships. Meanwhile, data streams decouple data producers and consumers, supporting them to be scheduled asynchronously as needed.

There are four key concepts in data streams: producer, consumer, stream, and element.

- Producer: The producer is the initiating end of an unbounded data stream, generating and sending data.
- Consumer: The consumer is the receiving end of an unbounded data stream, consuming data.
- Element: Data is sent and received between producers and consumers at the granularity of elements.
- Stream: Producers and consumers do not perceive each other directly, but are associated through streams. Producers send data to streams, and consumers subscribe to streams and receive data from them. An application can have multiple streams, distinguished by stream names.

Data streams support data interaction between multiple producers and multiple consumers. In actual scenarios, the most commonly used are one producer one consumer (one-to-one), multiple producers one consumer (many-to-one), and one producer multiple consumers (one-to-many).

## Usage Limitations

- Data streams have no persistence capability. When the sending node fails, data will be lost. Applications need to consider corresponding fault handling, such as restarting business logic.
- Data streams are all synchronous interfaces and do not have multiplexing capabilities like epoll.
- Streams decouple producers and consumers, which do not perceive each other. When a producer closes, the consumer cannot perceive it and needs to be handled at the business layer.

## Creating Streams

A stream represents the publish-subscribe interaction relationship between producers and consumers. Streams are implicitly created with the creation of producers or consumers, without requiring applications to explicitly create streams.

When creating a producer or consumer, you need to specify the stream it is associated with. Different streams are distinguished by stream names. If the stream already exists, the newly created producer or consumer will be associated with that stream. If the stream does not exist, when creating a producer or consumer, the system will implicitly create a new stream and associate it with the specified stream name.

After both producers and consumers on a stream are closed, if the stream has not been deleted through interface operations, the stream still exists, and applications can continue to associate new producers and consumers with this stream. You can specify the stream for automatic deletion when creating a producer (configure the `autoCleanup` option). When all producers and consumers on the stream are closed, the stream will be automatically deleted by the system.

:::::{tab-set}
::::{tab-item} Python

```python
import yr

yr.init()
stream_name = "this-stream"
try:
    # Configure stream for automatic deletion
    producer_config = yr.ProducerConfig(delay_flush_time=5, page_size=1024 * 1024, max_stream_size=1024 * 1024 * 1024, auto_clean_up=True)
    # Creating producer will implicitly create stream this-stream
    producer = yr.create_stream_producer(stream_name, producer_config)
    # Close producer, stream this-stream has no producers or consumers associated, will be automatically deleted
    producer.close()

    consumer_config = yr.SubscriptionConfig("local-consumer")
    # Creating consumer will implicitly create stream this-stream again
    consumer = yr.create_stream_consumer(stream_name, consumer_config)
    consumer.close()

    # Stream newly created by consumer needs to be explicitly deleted
    yr.delete_stream(stream_name)
except RuntimeError as exp:
    print(exp)

yr.finalize()
```

::::
::::{tab-item} C++

```cpp
#include <iostream>
#include "yr/yr.h"

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);
    std::string streamName = "this-stream";
    try {
        // Configure stream for automatic deletion
        YR::ProducerConf pConfig{.delayFlushTime=5, .pageSize=1024 * 1024ul, .maxStreamSize=1024 * 1024 * 1024ul, .autoCleanup=true};
        // Creating producer will implicitly create stream this-stream
        std::shared_ptr<YR::Producer> producer = YR::CreateProducer(streamName, pConfig);
        // Close producer, stream this-stream has no producers or consumers associated, will be automatically deleted
        producer->Close();

        YR::SubscriptionConfig sConfig("local-consumer", YR::SubscriptionType::STREAM);
        // Creating consumer will implicitly create stream this-stream again
        std::shared_ptr<YR::Consumer> consumer = YR::Subscribe(streamName, sConfig);
        consumer->Close();

        // Stream newly created by consumer needs to be explicitly deleted
        YR::DeleteStream(streamName);
    } catch (YR::Exception &e) {
        std::cout << e.what() << std::endl;
    }

    YR::Finalize();
    return 0;
}
```

::::
::::{tab-item} Java

```java
package com.example;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.List;

import org.yuanrong.api.YR;
import org.yuanrong.Config;
import org.yuanrong.exception.YRException;
import org.yuanrong.stream.Producer;
import org.yuanrong.stream.ProducerConfig;
import org.yuanrong.stream.Consumer;
import org.yuanrong.stream.SubscriptionConfig;
import org.yuanrong.stream.SubscriptionType;
import org.yuanrong.stream.Element;

public class Main {
    public static void main(String[] args) throws YRException {
        YR.init(new Config());

        String streamName = "this-stream";
        try {
            // Configure stream for automatic deletion
            ProducerConfig pConfig = ProducerConfig.builder()
                                         .delayFlushTimeMs(5L)
                                         .pageSizeByte(1024 * 1024L)
                                         .maxStreamSize(1024 * 1024 * 1024L)
                                         .autoCleanup(true).build();
            // Creating producer will implicitly create stream this-stream
            Producer producer = YR.createProducer(streamName, pConfig);
            // Close producer, stream this-stream has no producers or consumers associated, will be automatically deleted
            producer.close();

            SubscriptionConfig sConfig = SubscriptionConfig.builder().subscriptionName("local-consumer").build();
            // Creating consumer will implicitly create stream this-stream again
            Consumer consumer = YR.subscribe(streamName, sConfig);
            consumer.close();
           
            // Stream newly created by consumer needs to be explicitly deleted
            YR.deleteStream(streamName);
        } catch (YRException e) {
            e.printStackTrace();
        }

        YR.Finalize();
    }
}
```

::::
:::::

## Producing Stream Data

Producers can send data to streams. Data sent by producers will first be placed in a buffer, and the system refreshes the buffer to make it visible to consumers based on the Flush strategy configured by the producer (sending after a period of time or when the buffer is full). When the producer is no longer in use, it needs to be actively closed.

:::::{tab-set}
::::{tab-item} Python

```python
import yr

yr.init()
stream_name = "this-stream"
try:
    producer_config = yr.ProducerConfig(delay_flush_time=5, page_size=1024 * 1024, max_stream_size=1024 * 1024 * 1024, auto_clean_up=True)
    producer = yr.create_stream_producer(stream_name, producer_config)

    # Produce data
    element = yr.Element(value=b"hello", ele_id=0)
    producer.send(element)

    # Actively close producer
    producer.close()
except RuntimeError as exp:
    print(exp)

yr.finalize()
```

::::
::::{tab-item} C++

```cpp
#include <iostream>
#include "yr/yr.h"

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);
    std::string streamName = "this-stream";
    try {
        YR::ProducerConf pConfig{.delayFlushTime=5, .pageSize=1024 * 1024ul, .maxStreamSize=1024 * 1024 * 1024ul, .autoCleanup=true};
        std::shared_ptr<YR::Producer> producer = YR::CreateProducer(streamName, pConfig);

        // Produce data
        std::string data = "hello";
        YR::Element element((uint8_t *)(data.c_str()), data.size());
        producer->Send(element);

        // Actively close producer
        producer->Close();
    } catch (YR::Exception &e) {
        std::cout << e.what() << std::endl;
    }

    YR::Finalize();
    return 0;
}
```

::::
::::{tab-item} Java

```java
package com.example;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.List;

import org.yuanrong.api.YR;
import org.yuanrong.Config;
import org.yuanrong.exception.YRException;
import org.yuanrong.stream.Producer;
import org.yuanrong.stream.ProducerConfig;
import org.yuanrong.stream.Consumer;
import org.yuanrong.stream.SubscriptionConfig;
import org.yuanrong.stream.SubscriptionType;
import org.yuanrong.stream.Element;

public class Main {
    public static void main(String[] args) throws YRException {
        YR.init(new Config());

        String streamName = "this-stream";
        try {
            ProducerConfig pConfig = ProducerConfig.builder()
                                         .delayFlushTimeMs(5L)
                                         .pageSizeByte(1024 * 1024L)
                                         .maxStreamSize(1024 * 1024 * 1024L)
                                         .autoCleanup(true).build();
            Producer producer = YR.createProducer(streamName, pConfig);

            // Produce data
            String data = "hello";
            ByteBuffer buffer = ByteBuffer.wrap(data.getBytes());
            Element element = new Element(0L, buffer);
            producer.send(element);

            // Close producer
            producer.close();
        } catch (YRException e) {
            e.printStackTrace();
        }

        YR.Finalize();
    }
}
```

::::
:::::

## Consuming Stream Data

Consumers can receive data from streams and use the `Ack` method to confirm data reception. When the consumer is no longer in use, it needs to be actively closed.

After consumers receive data, they need to perform ACK operations on the data to confirm that the data and previously received data have been fully consumed. For data confirmed as fully consumed, the system will reclaim memory resources. Data streams provide automatic ACK functionality, which only requires configuring `autoAck` as `true` when creating the consumer. After each `Receive` operation by the application, the system will automatically confirm the previously received data, without requiring the application to actively call the `Ack` method.

:::{note}

After enabling automatic ACK, users must ensure that before each `Receive` call by the consumer, the data received last time has been fully consumed. After calling `Receive`, continuing to consume data received last time is undefined by the system.

:::

:::{hint}

When consumers call the `Receive` method, they obtain an `Element` object whose internal pointer points to the actual data. This data resides in the shared memory between the application function and the data system. Applications need to confirm through the ACK operation that the data has been consumed, at which point the data system can reclaim the memory resources occupied by that data. If applications do not call the ACK operation, the data system cannot determine whether the data has been consumed and thus cannot reclaim memory resources, ultimately leading to memory resource exhaustion and system exceptions.

:::

:::::{tab-set}
::::{tab-item} Python

```python
import yr

yr.init()
stream_name = "this-stream"
try:
    producer_config = yr.ProducerConfig(delay_flush_time=5, page_size=1024 * 1024, max_stream_size=1024 * 1024 * 1024, auto_clean_up=True)
    producer = yr.create_stream_producer(stream_name, producer_config)

    consumer_config = yr.SubscriptionConfig("local-consumer")
    consumer = yr.create_stream_consumer(stream_name, consumer_config)

    element = yr.Element(value=b"hello", ele_id=0)
    producer.send(element)

    # Consume data, wait until one data item arrives or 1 second timeout
    elements = consumer.receive(1000, 1)
    for e in elements:
        print("receive:" + e.data.decode())

    producer.close()
    # Actively close consumer
    consumer.close()
except RuntimeError as exp:
    print(exp)

yr.finalize()
```

::::
::::{tab-item} C++

```cpp
#include <iostream>
#include "yr/yr.h"

int main(int argc, char *argv[])
{
    YR::Init(YR::Config{}, argc, argv);
    std::string streamName = "this-stream";
    try {
        YR::ProducerConf pConfig{.delayFlushTime=5, .pageSize=1024 * 1024ul, .maxStreamSize=1024 * 1024 * 1024ul, .autoCleanup=true};
        std::shared_ptr<YR::Producer> producer = YR::CreateProducer(streamName, pConfig);

        YR::SubscriptionConfig sConfig("local-consumer", YR::SubscriptionType::STREAM);
        std::shared_ptr<YR::Consumer> consumer = YR::Subscribe(streamName, sConfig);

        std::string data = "hello";
        YR::Element element((uint8_t *)(data.c_str()), data.size());
        producer->Send(element);

        // Consume data, wait until one data item arrives or 1 second timeout
        std::vector<YR::Element> elements;
        consumer->Receive(1, 1000, elements);
        for (auto e : elements) {
            std::string str(reinterpret_cast<char *>(e.ptr), e.size);
            // Manual ACK
            consumer->Ack(e.id);
            std::cout << "receive: " << str << std::endl;
        }

        producer->Close();
        // Actively close consumer
        consumer->Close();
    } catch (YR::Exception &e) {
        std::cout << e.what() << std::endl;
    }

    YR::Finalize();
    return 0;
}
```

::::
::::{tab-item} Java

```java
package com.example;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.List;

import org.yuanrong.api.YR;
import org.yuanrong.Config;
import org.yuanrong.exception.YRException;
import org.yuanrong.stream.Producer;
import org.yuanrong.stream.ProducerConfig;
import org.yuanrong.stream.Consumer;
import org.yuanrong.stream.SubscriptionConfig;
import org.yuanrong.stream.SubscriptionType;
import org.yuanrong.stream.Element;

public class Main {
    public static void main(String[] args) throws YRException {
        YR.init(new Config());

        String streamName = "this-stream";
        try {
            ProducerConfig pConfig = ProducerConfig.builder()
                                         .delayFlushTimeMs(5L)
                                         .pageSizeByte(1024 * 1024L)
                                         .maxStreamSize(1024 * 1024 * 1024L)
                                         .autoCleanup(true).build();
            Producer producer = YR.createProducer(streamName, pConfig);

            SubscriptionConfig sConfig = SubscriptionConfig.builder().subscriptionName("local-consumer").build();
            Consumer consumer = YR.subscribe(streamName, sConfig);

            String data = "hello";
            ByteBuffer buffer = ByteBuffer.wrap(data.getBytes());
            Element element = new Element(0L, buffer);
            producer.send(element);

            // Consume data, wait until one data item arrives or 3 seconds timeout
            Charset charset = Charset.forName("UTF-8");
            List<Element> elements = consumer.receive(1, 3000);
            for (Element e : elements) {
                String str = charset.decode(e.getBuffer()).toString();
                // Manual ACK
                consumer.ack(e.getId());
                System.out.println("receive: " + str);
            }

            producer.close();
            // Actively close consumer
            consumer.close();
        } catch (YRException e) {
            e.printStackTrace();
        }

        YR.Finalize();
    }
}
```

::::
:::::

#include <stdio.h>

#define LEAK_RATE 1
#define BUCKET_SIZE 10
#define PACKET_SIZE 4

// Structure representing the leaky bucket
struct LeakyBucket
{
    int current_size;
};

// Initialize the leaky bucket
void initializeBucket(struct LeakyBucket *bucket)
{
    bucket->current_size = 0;
}

// Add packet to the leaky bucket
int addPacket(struct LeakyBucket *bucket, int packet_size)
{
    if ((bucket->current_size + packet_size) <= BUCKET_SIZE)
    {
        bucket->current_size += packet_size;
        return 1; // Packet added successfully
    }
    else
    {
        return 0; // Packet dropped due to overflow
    }
}

int main()
{
    struct LeakyBucket bucket;
    initializeBucket(&bucket);

    int arrival_times[] = {1, 2, 3, 5, 6, 8, 11, 12, 15, 16, 19};
    int num_packets = sizeof(arrival_times) / sizeof(arrival_times[0]);

    printf("Time\tPacket\tStatus\n");
    printf("---------------------\n");

    int current_time = 0;
    for (int i = 0; i < num_packets; i++)
    {
        // Simulate time passing
        while (current_time < arrival_times[i])
        {
            // Leak the bucket by one byte per second
            bucket.current_size -= LEAK_RATE;
            if (bucket.current_size < 0)
            {
                bucket.current_size = 0;
            }
            current_time++;

            // Display the current time and bucket status
            printf("%d\t-\tBucket Size: %d\n", current_time, bucket.current_size);
        }

        // Add the current packet to the bucket
        int conforming = addPacket(&bucket, PACKET_SIZE);

        printf("%d\t%d\t", current_time, i + 1);
        if (conforming)
        {
            printf("Conforming\n");
        }
        else
        {
            printf("Nonconforming\n");
        }
    }

    return 0;
}

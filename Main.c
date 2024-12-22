/* Sadeen
 *1212164
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

int numberOfCollision1 = 0;
int numberOfCollision2 = 0;

// Dictionary
typedef struct Node
{
    char Word[15]; // Strings with 15 characters
    char Meaning1[15];
    char Meaning2[15];
    char Meaning3[15];
} Node;
/*--------------------------------------------------------------------------------------------------------------------*/
typedef Node *Word;
/*--------------------------------------------------------------------------------------------------------------------*/
// Structure to represent a node in the hash table
typedef struct HashNode
{
    int flag; // -1= deleted, 0= empty, 1=Occupied.
    Word value;
    char key[15];
} HashNode;
/*--------------------------------------------------------------------------------------------------------------------*/
typedef HashNode *HashTable;
/*--------------------------------------------------------------------------------------------------------------------*/

// Function to initialize a hash table
void initializeHashTable(HashTable hashTable, int capacity)
{
    for (int i = 0; i < capacity; i++)
    {
        hashTable[i].flag = 0; // Marking all entries as empty
    }
}

bool isPrime(int num)
{
    if (num <= 2)
    {
        return true;
    }
    for (int i = 2; i < num; i++)
    {
        if (num % i == 0)
        {
            return false;
        }
    }
    return true;
}
// Function to hash string and calculate index
int HashFunction(char *key, int capacity) // For Linear and Double (Hash 1)
{
    int hashValue = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        hashValue += key[i];
    }

    return hashValue % capacity; // Prime < Capicity
}

// Function to insert a word into the hash table using linear probing
int LinearInsert(HashTable* hashTable, int *capacity, Word newWord)
{
    int index = HashFunction(newWord->Word, *capacity);
    int stepSize = 1; // Fixed step size, adjust as needed
    int maxAttempts = *capacity;


    int attempts = 0;

    while ((*hashTable)[index].flag == 1 && attempts < maxAttempts)
    {
        // Collision occurred, move to the next position with a fixed step size
        index = (index + stepSize) % (*capacity);
        attempts++;
    }
    numberOfCollision1 += attempts;

    // Rehashing for the full table
    if (attempts >= maxAttempts)
    {
        // Handle the case where it couldn't find an empty slot after multiple attempts
        printf("Error: Couldn't find an empty slot after %d attempts. Resizing the hash table.\n", attempts);

        // Resize the hash table to a Larger size
        int newCapacity = (*capacity) * 2; // Double the size
        *hashTable = realloc(*hashTable, newCapacity * sizeof(HashNode));
        if (hashTable == NULL)
        {
            exit(EXIT_FAILURE);
        }

        for (int i = (*capacity); i < newCapacity; i++)
        {
            (*hashTable)[i].flag = 0; // empty
        }

        *capacity = newCapacity;                           // update the capacity
        return LinearInsert(hashTable, capacity, newWord); // Retry the Insertion after Resizing
    }

    // insert the word at the calculated index
    (*hashTable)[index].flag = 1;
    strcpy((*hashTable)[index].key, newWord->Word);
    (*hashTable)[index].value = newWord;
    return attempts;
}

int secondaryHashFunction(int capacity, int i) // Hash2
{
    int temp = capacity - 1;
    while (temp >= 0)
    {
        if (isPrime(temp))
            break;
        temp--;
    }
    return temp - (i % temp);
}

int DoubleHashFunction(char *key, int capacity, int i)
{
    int Hash1 = HashFunction(key, capacity);
    int Hash2 = secondaryHashFunction(capacity, Hash1);

    return (Hash1 + i * Hash2) % capacity;
}

int DoubleInsert(HashTable* hashTable, int *capacity, Word newWord)
{
    int i = 0;

    int index = HashFunction(newWord->Word, *capacity);

    int attempts = 0, maxAttempts = *capacity;

    while ((*hashTable)[index].flag == 1 && attempts < maxAttempts)
    {
        // Collision occurred, move to the next position using double hashing
        i++;
        index = DoubleHashFunction(newWord->Word, *capacity, i);

        attempts++;
    }
    numberOfCollision2 += attempts;

    if (attempts >= maxAttempts)
    {
        // Handle the case where it couldn't find an empty slot after multiple attempts
        printf("Error: Couldn't find an empty slot after %d attempts. Resizing the hash table.\n", attempts);

        // Resize the hash table to a Larger Size
        int newCapacity = (*capacity) * 2; // Double the size, adjust as needed
        (*hashTable) = realloc(*hashTable, newCapacity * sizeof(HashNode));
        if (hashTable == NULL)
        {
            exit(EXIT_FAILURE);
        }

        for (int i = (*capacity); i < newCapacity; i++)
        {
            (*hashTable)[i].flag = 0; // Marking all entries as empty
        }

        *capacity = newCapacity;                           // Update the capacity
        return DoubleInsert(hashTable, capacity, newWord); // Retry the Insertion after Resizing
    }
    else
    {
        // insert the word at the calculated index
        (*hashTable)[index].flag = 1;
        strcpy((*hashTable)[index].key, newWord->Word);
        (*hashTable)[index].value = newWord;
        return attempts;
    }
}

void Menu()
{
    printf("\nHELLO, Choose one :\n");
    printf("  1- Print hashed tables (including empty spots).\n");
    printf("  2- Print out table size and the load factor.\n");
    printf("  3- Print out the used hash functions.\n");
    printf("  4- Insert a new record to hash table (insertion will be done on both hash tables).\n");
    printf("  5- Search for a specific word (specify which table to search in).\n");
    printf("  6- Delete a specific word (from both tables).\n");
    printf("  7- Compare between the two methods in terms of number of\n     collisions occurred. Here you have to print the number of\n"
           "     collisions occurred (if happened) once every new word is inserted.\n");
    printf("  8- Save hash table back to a file named saved_words.txt (of the double hashing) \n");
}

void ReadFromFile(HashTable linearHashTable, HashTable doubleHashTable, int *size, int *size2)
{
    FILE *file = fopen("words.txt", "r");
    if (file == NULL)
    {
        printf("File Does not exist\n");
        return;
    }

    Node *newWord = (Node *)malloc(sizeof(Node));

    if (newWord == NULL)
    {
        printf("Memory allocation failed\n");
        fclose(file);
        return;
    }
    //    !Important Note!
    /* Don't use this fscanf to read the words, You can use strtok Function(The best way) */
    int check = fscanf(file, "%[^:]:%[^#]#%[^#]#%[^\n]\n", newWord->Word,
                       newWord->Meaning1, newWord->Meaning2, newWord->Meaning3);


    while ( check != EOF)
    {
        // Insert into linear hash table
        LinearInsert(&linearHashTable, size, newWord);

        // Insert into double hash table
        DoubleInsert(&doubleHashTable, size2, newWord);
        newWord = (Node *)malloc(sizeof(Node));

        check = fscanf(file, "%[^:]:%[^#]#%[^#]#%[^\n]\n", newWord->Word,
                       newWord->Meaning1, newWord->Meaning2, newWord->Meaning3);
    }
    free(newWord);
    fclose(file);
}

int findPrime(int num) // Used in SizeTable()
{
    int prime = num + 1;
    while (!isPrime(prime))
    {
        prime++;
    }
    return ((prime*2)+15);
}

// Count the number of lines in a file to determine the size table.
int SizeTable()
{
    FILE *file;
    char *line = NULL;
    int len = 0; //Unsigned Integer
    int read; //signed Integer
    int count = 0;

    // Open the file for Reading
    file = fopen("words.txt", "r");

    // check if the file is opened successfully
    if (file == NULL)
    {
        printf("Unable to open the file.\n");
        return 1;
    }

    // Read Each Line and Count Non Empty Lines
    while ((read = getline(&line, &len, file)) != -1)/*No Error here, use (VS Code Not CodeBlocks Or Clion) to run it*/
    {
        if (strcmp(line, "\n") != 0 && strcmp(line, "\r\n") != 0)
        {
            count++;
        }
    }

    // close the file
    fclose(file);

    // free  allocated memory
    if (count != 0)
        free(line);

    if (!isPrime(count))
        return findPrime(count);

    else
        return (count*2)+15;
}

void printHashTable(HashTable hashTable, int capacity)
{
    printf("Index\t\tKey\t\tValue\n");

    for (int i = 0; i < capacity; i++)
    {
        printf("  %d\t", i);

        if (hashTable[i].flag == 1)
            printf("\t%s\t\t%s   %s   %s\n", hashTable[i].value->Word, hashTable[i].value->Meaning1, hashTable[i].value->Meaning2, hashTable[i].value->Meaning3);

        else if (hashTable[i].flag == -1)
            printf("\tDeleted\n");

        else
            printf("\n"); // For empty slot

    }
    printf("\n");
}

// Function to count the number of words in the Linear hash table
int countWordsLinear(HashTable hashTable, int capacity)
{
    int count = 0;
    for (int i = 0; i < capacity; i++)
    {
        if (hashTable[i].flag == 1)
        {
            count++;
        }
    }
    return count;
}

// Function to count the number of words in the Double hash table
int countWordsDouble(HashTable hashTable, int capacity)
{
    int count = 0;
    for (int i = 0; i < capacity; i++)
    {
        if (hashTable[i].flag == 1)
        {
            count++;
        }
    }
    return count;
}
void printTableInfo(HashTable hashTable, int capacity, int size, float loadFactor)
{
    printf("Table Size: %d\n", size);
    printf("Capacity: %d\n", capacity);
    printf("Load Factor: %.2f\n", loadFactor);
    printf("\n");
}

int LinearHashFind(HashTable hashTable, char *key, int capacity)
{
    int index = HashFunction(key, capacity); // Calculate the initial index

    // Linear probing to find the key
    while (hashTable[index].flag != 0)
    {
        if (hashTable[index].flag == 1 && strcmp(hashTable[index].key, key) == 0)
        {
            // Key found
            return index;
        }

        // Move to the next position
        index = (index + 1) % capacity;
    }

    // Key not found
    return -1;
}

int DoubleHashFind(HashTable hashTable, char *key, int capacity)
{
    int i = 0;
    int index = HashFunction(key, capacity); // Calculate the initial index

    // Double probing to find the key
    while (hashTable[index].flag != 0)
    {
        //index = DoubleHashFunction(key, capacity,i);
        if (hashTable[index].flag == 1 && strcmp(hashTable[index].key, key) == 0)
        {
            // Key found
            return index;
        }
        // Move to the next position
        i++;

        index = DoubleHashFunction(key, capacity, i);
    }
    // Key not found
    return -1;
}

// Function for Deleting a Word from both hash tables
int DeleteWord(HashTable linearHashTable, HashTable doubleHashTable, char *key, int sizeLinear, int sizeDouble)
{
    int linearIndex = LinearHashFind(linearHashTable, key, sizeLinear);
    int doubleIndex = DoubleHashFind(doubleHashTable, key, sizeDouble);

    if (linearIndex != -1)
    {
        linearHashTable[linearIndex].flag = -1;   // Mark the entry as deleted
        free(linearHashTable[linearIndex].value); // Free the memory allocated for the word
        printf("Word \"%s\" deleted from Linear Hash Table.\n", key);
    }
    else
        printf("Word \"%s\" not found in Linear Hash Table.\n", key);
    /*-------------------------------------------------------------------------*/


    // Deleting from Double Hash Table
    if (doubleIndex != -1)
    {
        doubleHashTable[doubleIndex].flag = -1; // -1: deleted
        printf("Word \"%s\" deleted from Double Hash Table.\n", key);
    }
    else
        printf("Word \"%s\" not found in Double Hash Table.\n", key);

    return 0;
}


// Load Factor Calculation
float LoadFactor(int capacity, int size)
{

    if (size == 0)
    {
        // Handle division by zero
        return 0.0;
    }

    return (float)capacity / (float)size;
}

void SaveToFile(HashTable doubleHashTable, int capacity)
{
    FILE *file = fopen("saved_words.txt", "w");

    if (file == NULL)
    {
        printf("Error creating the file.\n");
        return;
    }

    fprintf(file, "------------------------Double Hash Table------------------------\n");
    fprintf(file, "Index\t\t\t  \tKey\t\t    \tValue\n");

    for (int i = 0; i < capacity; i++)
    {
        fprintf(file, "  %d\t", i);
        if (doubleHashTable[i].flag == 1)
        {
            // For 3 Meanings
            if (doubleHashTable[i].value->Meaning1 != "\0" && doubleHashTable[i].value->Meaning2 != "\0" &&
                doubleHashTable[i].value->Meaning3 != "\0")
                fprintf(file, "          %s\t\t    %s   %s   %s\n", doubleHashTable[i].value->Word,
                        doubleHashTable[i].value->Meaning1, doubleHashTable[i].value->Meaning2,
                        doubleHashTable[i].value->Meaning3);

            // For two Meanings
            if (doubleHashTable[i].value->Meaning1 != "\0" && doubleHashTable[i].value->Meaning2 != "\0" &&
                doubleHashTable[i].value->Meaning3 == "\0")
                fprintf(file, "          %s\t\t    %s   %s\n", doubleHashTable[i].value->Word,
                        doubleHashTable[i].value->Meaning1, doubleHashTable[i].value->Meaning2);

            // For one Meaning
            if (doubleHashTable[i].value->Meaning1 != "\0" && doubleHashTable[i].value->Meaning2 == "\0" &&
                doubleHashTable[i].value->Meaning3 == "\0")
                fprintf(file, "          %s\t\t    %s\n", doubleHashTable[i].value->Word,
                        doubleHashTable[i].value->Meaning1);
        }

        else if (doubleHashTable[i].flag == -1)
        {
            fprintf(file, "Deleted\n");
        }
        else
        {
            fprintf(file, " \n");
        }
    }

    fclose(file);
    printf("Double Hash Table saved to 'saved_words.txt' successfully.\n");
}

int main()
{
    int s;
    int sizeLinear = SizeTable();

    int sizeDouble = sizeLinear;

    HashTable linearHashTable;
    HashTable doubleHashTable;

    // Allocate memory for hash tables based on determined capacities
    linearHashTable = (HashNode *)malloc(sizeof(HashNode) * sizeLinear);
    doubleHashTable = (HashNode *)malloc(sizeof(HashNode) * sizeLinear);

    initializeHashTable(linearHashTable, sizeLinear);
    initializeHashTable(doubleHashTable, sizeDouble);

    ReadFromFile(linearHashTable, doubleHashTable, &sizeLinear, &sizeDouble);

    do
    {
        Menu();
        scanf("%d", &s);

        switch (s)
        {

            case 1:
                printf("------------------------Linear Hash Table------------------------\n");
                printHashTable(linearHashTable, sizeLinear);
                printf("------------------------Double Hash Table------------------------\n");
                printHashTable(doubleHashTable, sizeDouble);
                break;

            case 2:
                printf("Linear Hash Table Info:\n");
                int capacity = countWordsLinear(linearHashTable, sizeLinear);
                printTableInfo(linearHashTable, capacity, sizeLinear, LoadFactor(capacity, sizeLinear));

                printf("Double Hash Table Info:\n");
                capacity = countWordsDouble(doubleHashTable, sizeDouble);
                printTableInfo(doubleHashTable, capacity, sizeDouble, LoadFactor(capacity, sizeDouble));
                break;

            case 3:
                printf("Linear Probing Function : h(x) = (i+1) %% (Table Size)\n");
                printf("Double Probing Function : h(x) = (Hash1(key)+Hash2(key)) %% (Table Size)\n");
                break;

            case 4:
            {
                Word newWord = (Word)malloc(sizeof(Node));
                if (newWord == NULL)
                {
                    printf("Memory allocation failed\n");
                    break;
                }

                int meaningsNumber;
                int select;

                printf("Enter the word and it's Numebr of synonmys(Maximum 3 meanings): ");
                scanf("%s\t%d", newWord->Word, &meaningsNumber);

                for (int i = 0; i < meaningsNumber; i++)
                {
                    if (i == 0)
                    {
                        printf("Enter the First Meaning: ");
                        scanf("%s", newWord->Meaning1);
                    }
                    if (i == 1)
                    {
                        printf("Enter the Second Meaning: ");
                        scanf("%s", newWord->Meaning2);
                    }
                    if (i == 2)
                    {
                        printf("Enter the Third Meaning: ");
                        scanf("%s", newWord->Meaning3);
                    }
                }
                //                printf("Enter the first meaning: ");
                //                scanf("%s", newWord->Meaning1);
                //                printf("Enter the second meaning: ");
                //                scanf("%s", newWord->Meaning2);
                //                printf("Enter the third meaning: ");
                //                scanf("%s", newWord->Meaning3);

                LinearInsert(&linearHashTable, &sizeLinear, newWord);
                DoubleInsert(&doubleHashTable, &sizeDouble, newWord);

                break;
            }

            case 5:
            {
                char searchKey[15];
                int select = 0;
                printf("Enter number tha hash table you want to search in it:\n 1: Linear\t 2: Double\n");
                scanf("%d", &select);

                printf("\nEnter the word to search: ");
                scanf("%s", searchKey);

                if (select == 1)
                {
                    int linearIndex = LinearHashFind(linearHashTable, searchKey, sizeLinear);
                    if (linearIndex != -1)
                    {
                        printf("Your word Found in Linear Hash Table :)\n");
                        printf("Index\t\t  Key\t\t    Value\n");
                        printf("  %d\t", linearIndex);
                        printf("          %s\t\t    %s   %s   %s\n", linearHashTable[linearIndex].value->Word, linearHashTable[linearIndex].value->Meaning1, linearHashTable[linearIndex].value->Meaning2, linearHashTable[linearIndex].value->Meaning3);
                    }

                    else
                        printf("Not found in Linear Hash Table :(\n");
                }

                if (select == 2)
                {
                    int doubleIndex = DoubleHashFind(doubleHashTable, searchKey, sizeDouble);

                    // int doubleIndex = DeleteWord(doubleHashTable, searchKey,sizeDouble,false);
                    if (doubleIndex != -1)
                    {
                        printf("search key : %s\n", searchKey);
                        printf("Your word Found in Double Hash Table :)\n");
                        printf("Index\t\t  Key\t\t    Value\n");
                        printf("  %d\t", doubleIndex);
                        printf("          %s\t\t    %s   %s   %s\n", doubleHashTable[doubleIndex].value->Word, doubleHashTable[doubleIndex].value->Meaning1, doubleHashTable[doubleIndex].value->Meaning2, doubleHashTable[doubleIndex].value->Meaning3);
                    }

                    else
                        printf("Not found in Double Hash Table :(\n");
                }
                break;
            }
            case 6:
            {
                char deleteKey[15];
                printf("Enter the word to delete: ");
                scanf("%s", deleteKey);

                DeleteWord(linearHashTable, doubleHashTable, deleteKey, sizeLinear, sizeDouble);
                break;
            }

            case 7:
                printf("Table 1 \"Linear Probing\" : Number Of Collision = %d\n", numberOfCollision2);
                printf("Table 2 \"Double Probing\" : Number Of Collision = %d\n", numberOfCollision1);
                break;

            case 8:
                SaveToFile(doubleHashTable, sizeDouble);
                break;

            default:
                printf("Invalid Choice. Please try again.\n");
                break;
        }
    } while (1);
}





#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count, candidate_count, i, j, k;
bool winner;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void mergesort(pair pairs[pair_count], int p, int r);
void merge(pair pairs[pair_count], int p, int q, int r);
bool cause_cycle(int subject, int target);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (i = 0; i < candidate_count; i++)
    {
        for (j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int v = 0; v < voter_count; v++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }
        record_preferences(ranks);
        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();

    return 0;

}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    //check candidate name is valid and update ranks array
    for (i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (j = 0; j < candidate_count; j++)
    {
        for (k = j + 1; k < candidate_count; k++)
        {
            preferences[ranks[j]][ranks[k]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    bool exists;

    //each column in preferences
    for (i = 0; i < candidate_count; i++)
    {
        //each row in preferences
        for (j = 0; j < candidate_count; j++)
        {
            //reset check for if a pair exists
            exists = false;

            //if more prefer i to j than j to i, i is winner and j is loser
            if (preferences[i][j] > preferences[j][i])
            {
                //check pair doesn't already exist
                for (k = 0; k < pair_count; k++)
                {
                    if (pairs[k].winner == i && pairs[k].loser == j)
                    {
                        exists = true;
                    }
                }
                if (exists == false)
                {
                    pairs[pair_count].winner = i;
                    pairs[pair_count].loser = j;
                    pair_count++;
                }
            }
            //if more prefer j to i than i to j, j is winner and i is loser
            else if (preferences[j][i] > preferences[i][j])
            {
                //check pair doesn't already exist
                for (k = 0; k < pair_count; k++)
                {
                    if (pairs[k].winner == j && pairs[k].loser == i)
                    {
                        exists = true;
                    }
                }
                if (exists == false)
                {
                    pairs[pair_count].winner = j;
                    pairs[pair_count].loser = i;
                    pair_count++;
                }
            }
        }
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    //merge sort the pairs array from 0 to last element, referring to preferences[] for strength of victory
    mergesort(pairs, 0, pair_count - 1);
}


// function to determine if adding a line to the locked[] graph will cause a cycle
// subject is current candidate, target is the losing candidate in the considered pair
bool cause_cycle(int subject, int target)
{
    // if the subject is the target, it will lead back to the subject and cause a cycle
    if (subject == target)
    {
        return true;
    }
    else
    {
        for (int n = 0; n < candidate_count; n++)
        {
            if (locked[n][subject] == true)
            {
                // recursion to check each layer against the same target
                if (cause_cycle(n, target))
                {
                    return true;
                }
            }
        }
        // if target does not lead back to subject at any stage
        return false;
    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (i = 0; i < pair_count; i++)
    {
        if (!cause_cycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
}

// Print the winner of the election
void print_winner(void)
{
    // check all candidates in locked[]
    for (i = 0; i < candidate_count; i++)
    {
        //reset winner when checking each candidate
        winner = true;

        for (j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                winner = false;
            }
        }
        if (winner == true)
        {
            printf("%s\n", candidates[i]);
        }
    }
}


//input array[], p is first element (0), r is last element (length of array[])
void mergesort(pair pairs1[pair_count], int p, int r)
{
    if (p < r)
    {
        int q = (p + r) / 2;
        mergesort(pairs1, p, q); // sort left half of pairs1
        mergesort(pairs1, q + 1, r); //sort right half of pairs1
        merge(pairs1, p, q, r); //merge left (p to q) and right (q+1 to r) halves
    }
}

//p is first element of left subarray, q is last element of left subarray, r is last element of right subarray
void merge(pair pairs1[pair_count], int p, int q, int r)
{
    //get length for left array L
    int n1 = q - p + 1;
    //get length for right array R
    int n2 = r - q;

    //declare arrays for left and right halves
    pair L[n1], R[n2];

    //fill left array
    for (i = 0; i < n1; i++)
    {
        L[i] = pairs1[p + i];
    }

    //fill right array
    for (j = 0; j < n2; j++)
    {
        R[j] = pairs1[q + j + 1];
    }

    //track index of sub arrays and main array
    i = 0;
    j = 0;
    k = p;

    //until we reach the end of the left or right array
    while (i < n1 && j < n2 && k < r + 1)
    {
        //pick the larger strength of victory between L and R and put in array
        if (preferences[L[i].winner][L[i].loser] >= preferences[R[j].winner][R[j].loser])
        {
            pairs1[k] = L[i];
            i++;
        }
        else
        {
            pairs1[k] = R[j];
            j++;
        }
        k++;
    }
    //once we run out of elements in either the left of right array, copy the remaining elements into array[k]
    while (i < n1)
    {
        pairs1[k] = L[i];
        i++;
        k++;
    }
    while (j < n2)
    {
        pairs1[k] = R[j];
        j++;
        k++;
    }
}

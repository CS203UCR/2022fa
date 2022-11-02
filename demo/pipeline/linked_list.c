struct node
{
    int value;
    struct node *next;
};

int depth_of_list(struct node *start)
{
    int number_of_nodes = 0;
    struct node *node = start;
    while(node)
    {
        node = node->next;
        number_of_nodes++;
        
    }
    return number_of_nodes;
}
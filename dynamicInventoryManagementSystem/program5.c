#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct productData
{
    int id;
    char *name;
    float price;
    int quantity;
};

void readProductId(struct productData *productInventory, int productCount) {
    while (1)
    {
        if (scanf("%d", &(productInventory)[productCount - 1].id) == 1) break;
        printf("\nInvalid Input!, try again.\n");
        while (getchar() != '\n');
    }
}

void readProductQuantity(struct productData *productInventory, int productCount) {
    while (1)
    {
        if (scanf("%d", &(productInventory)[productCount - 1].quantity) == 1) break;
        printf("\nInvalid Input!, try again.\n");
        while (getchar() != '\n');
    }
}

void addNewProduct(struct productData **productInventory, int *productCount)
{
    (*productCount)++;
    struct productData *temporaryPointer = (struct productData *)realloc(*productInventory, (*productCount) * sizeof(struct productData));

    if (temporaryPointer == NULL)
    {
        printf("\nmemory reallocation failed!\n");
        exit(1);
    }

    *productInventory = temporaryPointer;

    printf("Enter new product details:\n");
    printf("Product ID: ");
    readProductId(*productInventory, *productCount);

    (*productInventory)[*productCount - 1].name = (char *)malloc(100 * sizeof(char));
    if ((*productInventory)[*productCount - 1].name == NULL)
    {
        printf("Memory allocation failed for product name!\n");
        exit(1);
    }
    printf("Product Name: ");
    scanf(" %[^\n]", (*productInventory)[*productCount - 1].name);
    printf("Product Price: ");
    while (1)
    {
        if (scanf("%f", &(*productInventory)[*productCount - 1].price) == 1)
            break;
        printf("\nInvalid Input!, try again.\n");
        while (getchar() != '\n');
    }

    printf("Product Quantity: ");
    readProductQuantity(*productInventory, *productCount);
    printf("Product added successfully!\n");
}

void viewAllProducts(struct productData *productInventory, int productCount)
{
    if (productCount == 0)
    {
        printf("No products available!\n");
        return;
    }

    printf("========= PRODUCT LIST =========\n");
    for (int i = 0; i < productCount; i++)
    {
        printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               productInventory[i].id, productInventory[i].name, productInventory[i].price, productInventory[i].quantity);
    }
}

void updateProductQuantity(struct productData *productInventory, int productCount)
{
    int productID, found = 0;
    printf("Enter Product ID to update quantity: ");

    while (1)
    {
        if (scanf("%d", &productID) == 1)
            break;
        printf("\nInvalid Input!, try again.\n");
        while (getchar() != '\n');
    }

    for (int i = 0; i < productCount; i++)
    {
        if (productInventory[i].id == productID)
        {
            printf("Enter new Quantity: ");
            while (1)
            {
                if (scanf("%d", &productInventory[i].quantity) == 1)
                    break;
                printf("\nInvalid Input!, try again.\n");
                while (getchar() != '\n');
            }
            printf("Quantity updated successfully!\n");
            found = 1;
            break;
        }
    }
    if (!found)
        printf("Product not found!\n");
}

void searchByID(struct productData *productInventory, int productCount)
{
    int productID, found = 0;
    printf("Enter Product ID to search: ");
    scanf("%d", &productID);

    for (int i = 0; i < productCount; i++)
    {
        if (productInventory[i].id == productID)
        {
            printf("Product Found: Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   productInventory[i].id, productInventory[i].name, productInventory[i].price, productInventory[i].quantity);
            found = 1;
            break;
        }
    }
    if (!found)
        printf("Product not found!\n");
}

void searchByPriceRange(struct productData *productInventory, int productCount)
{
    float min, max;
    int found = 0;
    printf("Enter minimum price: ");
    scanf("%f", &min);
    printf("Enter maximum price: ");
    scanf("%f", &max);

    printf("Products in price range:\n");
    for (int i = 0; i < productCount; i++)
    {
        if (productInventory[i].price >= min && productInventory[i].price <= max)
        {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   productInventory[i].id, productInventory[i].name, productInventory[i].price, productInventory[i].quantity);
            found = 1;
        }
    }
    if (!found)
        printf("No products found in this range!\n");
}

void deleteProduct(struct productData **productInventory, int *productCount)
{
    int productID, found = 0, productPosition = -1;
    printf("Enter Product ID to delete: ");
    scanf("%d", &productID);

    for (int i = 0; i < *productCount; i++)
    {
        if ((*productInventory)[i].id == productID)
        {
            productPosition = i;
            found = 1;
            break;
        }
    }

    if (found)
    {
        free((*productInventory)[productPosition].name);
        for (int i = productPosition; i < (*productCount) - 1; i++)
        {
            (*productInventory)[i] = (*productInventory)[i + 1];
        }

        (*productCount)--;
        struct productData *temporaryPointer = (struct productData *)realloc(*productInventory, (*productCount) * sizeof(struct productData));

        if (temporaryPointer == NULL) {
            printf("\nmemory reallocation failed!\n");
            exit(1);
        }
        *productInventory = temporaryPointer;
        printf("Product deleted successfully!\n");
    }
    else
    {
        printf("Product not found!\n");
    }
}

int isPartialMatch(char *productName, char *userSearch)
{
    int i, j;
    int lengthOfProductName = strlen(productName);
    int lengthOfUserSearch = strlen(userSearch);

    for (i = 0; i <= lengthOfProductName - lengthOfUserSearch; i++)
    {
        for (j = 0; j < lengthOfUserSearch; j++)
        {
            if (productName[i + j] != userSearch[j]) break;
        }
        if (j == lengthOfUserSearch)
            return 1;
    }
    return 0;
}

void searchByName(struct productData *productInventory, int productCount)
{
    char userSearch[100];
    int found = 0;

    printf("Enter product name to search: ");
    scanf(" %[^\n]", userSearch);

    printf("\nProducts Found:\n");
    for (int i = 0; i < productCount; i++)
    {
        if (isPartialMatch(productInventory[i].name, userSearch))
        {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   productInventory[i].id, productInventory[i].name, productInventory[i].price, productInventory[i].quantity);
            found = 1;
        }
    }

    if (!found)
        printf("No product found matching \"%s\".\n", userSearch);
}

void inputProductCount(int *numberOfProducts)
{
    while (1)
    {
        printf("Enter the number of products:\n");
        if (scanf("%d", numberOfProducts) == 1)
        {
            break;
        }
        printf("\nInvalid Input!, Try again\n");
        while (getchar() != '\n');
    }
}

void readProductProperties(struct productData *productInventory, int numberOfProducts)
{
    for (int i = 0; i < numberOfProducts; i++)
    {
        printf("Enter details for product %d:\n", i + 1);
        printf("Product ID: ");
        while (1)
        {
            if (scanf("%d", &productInventory[i].id) == 1) break;
            while (getchar() != '\n');
            printf("\nInvalid Input!, Try again\n");
        }
        productInventory[i].name = (char *)malloc(100 * sizeof(char));
        if (productInventory[i].name == NULL)
        {
            printf("Memory allocation failed for product name!\n");
            exit(1);
        }

        printf("Product Name: ");
        scanf(" %[^\n]", productInventory[i].name);
        printf("Product Price: ");
        while (1) {
            if (scanf("%f", &productInventory[i].price) == 1) break;
            while (getchar() != '\n');
            printf("\nInvalid Input!, Try again\n");
        }
        printf("Product Quantity: ");
        while (1)
        {
            if (scanf("%d", &productInventory[i].quantity) == 1)
            {
                break;
            }
            while (getchar() != '\n');
            printf("\nInvalid Input!, Try again\n");
        }
    }
}

void takeInput(int *variable) {
    while (1) {
        if (scanf("%d", variable) == 1) break;
        while (getchar() != '\n');
    }
}

void userChoices(struct productData **productInventory, int *numberOfProducts)
{
    int userChoice;
    do {
        printf("\nEnter your choice:\n1: Add New Product\n2: View all Product\n3: Update quantity of an Product\n4: Search for a Product by ID\n5: Search for an product by name\n6: Search for Products by Price Range\n7: Delete a Product by ID\n8: Exit the Program\n");
        takeInput(&userChoice);
        switch (userChoice) {
        case 1:
            addNewProduct(productInventory, numberOfProducts);
            break;
        case 2:
            viewAllProducts(*productInventory, *numberOfProducts);
            break;
        case 3:
            updateProductQuantity(*productInventory, *numberOfProducts);
            break;
        case 4:
            searchByID(*productInventory, *numberOfProducts);
            break;
        case 5:
            searchByName(*productInventory, *numberOfProducts);
            break;
        case 6:
            searchByPriceRange(*productInventory, *numberOfProducts);
            break;
        case 7:
            deleteProduct(productInventory, numberOfProducts);
            break;
        case 8:
            for(int i = 0; i < *numberOfProducts; i++) {
                free((*productInventory)[i].name);
            }
            free(*productInventory);
            printf("Memory released successfully! Exiting program...\n");
            break;
        default:
            printf("Invalid choice!\n");
        }
    } while (userChoice != 8);
}

int main()
{
    int numberOfProducts;
    inputProductCount(&numberOfProducts);

    struct productData *productInventory = NULL;
    productInventory = (struct productData *)calloc(numberOfProducts, sizeof(struct productData));

    if (productInventory == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    readProductProperties(productInventory, numberOfProducts);
    userChoices(&productInventory, &numberOfProducts);
    return 0;
}
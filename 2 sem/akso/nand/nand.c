#include <stdbool.h>                      // Library providing the bool data type and true/false values
#include <stddef.h>                       // Defines a set of standard macros for NULL
#include <sys/types.h>                    // Defines a collection of data types used in system calls
#include <errno.h>                        // Defines macros for reporting and retrieving error conditions
#include <stdlib.h>                       // General utilities library for memory allocation, random numbers, etc.
#include <stdio.h>                        // Standard I/O library for input and output operations
#include <assert.h>                       // Standard debugging library for assertion checking

#include "nand.h"                         // Header file for custom NAND gate implementation

#define max(a, b)((a) > (b) ? (a) : (b))  // Macro for getting the maximum of two values

typedef struct nand {
    struct nand **linked_inputs;             // Array of pointers to NAND gates connected as inputs
    struct nand **linked_outputs;            // Array of pointers to NAND gates connected as outputs

    unsigned int number_of_linked_inputs;    // Number of input connections
    unsigned int number_of_linked_outputs;   // Number of output connections
    unsigned int size_of_array;              // Size of the output array

    bool type;                      // Type of the NAND gate (true for gate, false for signal)
    bool if_estimated_path;         // Indicates if the critical path length is estimated
    bool if_estimated_value;        // Indicates if the output value is estimated
    bool estimated_value;           // Estimated output value
    int length_of_critical_path;    // Length of the critical path 
    int checked;                    // Flag used for cycle detection

    bool const *pointer;            // Pointer to the value of a signal gate
} nand_t;

static int     nand_output_reduction(nand_t * g_in, nand_t * g_out, bool option) {
    // Reduce the output connections of a NAND gate.
    // 'option' is used for breaking the loop if code is not supposed delate 'g_in' from every output.
    for (unsigned int i = 0; i < g_out->size_of_array; i++) {
        if (g_out->linked_outputs[i] == g_in) {
            // If the output connection matches the specified NAND gate,
            g_out->linked_outputs[i] = NULL;
            // If p1 is true, break the loop after the first match
            if (option == true) break;
        }
    }
    // Decrement the count of linked outputs for the NAND gate
    g_out->number_of_linked_outputs--;
    return 0;  // Return 0 to indicate success
}
static int     nand_input_reduction(nand_t * g_in, nand_t * g_out) {
    // Reduce the input connections of a NAND gate.
    if (g_out != NULL) {
        for (unsigned int i = 0; i < g_out->number_of_linked_inputs; i++) {
            // Iterate through the input connections of the output NAND gate
            if (g_out->linked_inputs[i] != NULL && g_out->linked_inputs[i] == g_in) {
                // If the input connection matches the specified NAND gate,
                g_out->linked_inputs[i] = NULL;
            }
        }
    }
    return 0;  // Return 0 to indicate success
}
static int     nand_output_addition(nand_t * g_in, nand_t * g_out) {
    // Add an output connection to a NAND gate.
    for (unsigned int i = 0; i < g_out->size_of_array; i++) {
        // Iterate through the output connections of the NAND gate
        if (g_out->linked_outputs[i] == NULL) {
            // If an empty slot is found, add the input NAND gate as an output
            g_out->number_of_linked_outputs++;
            g_out->linked_outputs[i] = g_in;
            return 0;  // Return 0 to indicate success
        }
    }
    // If no empty slot is found, reallocate memory to increase the size of the array
    nand_t **result = (nand_t **) realloc(g_out->linked_outputs, sizeof(nand_t *) * 2*(g_out->size_of_array + 1));
    if (result == NULL) {
        errno = ENOMEM;
        free(result);
        return -1;  // Return -1 to indicate failure
    }
    // Update the NAND gate's output connections and increase the size of the array
    g_out->linked_outputs = result;
    g_out->linked_outputs[g_out->number_of_linked_outputs] = g_in;
    g_out->number_of_linked_outputs++;
    g_out->size_of_array++;
    return 0;  // Return 0 to indicate success
}
nand_t         *nand_new(unsigned n) {
    // Create a new NAND gate.
    nand_t *result = (nand_t *) malloc(sizeof(nand_t));
    if (result == NULL) {
        // If memory allocation fails, set errno to ENOMEM and return NULL
        errno = ENOMEM;
        return NULL;
    }
    result->linked_inputs = (nand_t **) malloc(sizeof(nand_t *) * n);
    if (result->linked_inputs == NULL) {
        // If memory allocation fails, set errno to ENOMEM, free memory, and return NULL
        errno = ENOMEM;
        free(result);
        return NULL;
    }
    // Initialize the properties of the NAND gate
    result->number_of_linked_inputs = n;
    for (unsigned int i = 0; i < n; i++) result->linked_inputs[i] = NULL;
    result->number_of_linked_outputs = 0;
    result->linked_outputs = NULL;
    result->size_of_array = 0;
    result->checked = 0;
    result->estimated_value = false;
    result->if_estimated_path = false;
    result->if_estimated_value = false;
    result->type = true;
    result->length_of_critical_path = 1;
    result->pointer = NULL;
    return result;
}
nand_t         *nand_new_signal(bool const * value) {
    // Create a new NAND gate to represent a signal.
    nand_t *result = (nand_t *) malloc(sizeof(nand_t));
    if (result == NULL) {
        // If memory allocation fails, set errno to ENOMEM and return NULL
        errno = ENOMEM;
        free(result);
        return NULL;
    }
    // Initialize the properties of the signal NAND gate
    result->number_of_linked_inputs = 0;
    result->number_of_linked_outputs = 0;
    result->if_estimated_path = true;
    result->if_estimated_value = true;
    bool signal_value = *value;
    result->estimated_value = signal_value;
    result->length_of_critical_path = 0;
    result->linked_outputs = NULL;
    result->linked_inputs = NULL;
    result->type = false;
    result->checked = 2;
    result->size_of_array = 0;
    result->pointer = value;
    return result;
}
static void    nand_signal_delete(nand_t * g) {
    // Delete a signal NAND gate.
    if (g != NULL && g->type == false) {
        // Check if the NAND gate is not NULL and is of type signal
        free(g); 
    }
}
void           nand_delete(nand_t * g) {
    // Delete a NAND gate and its connections.
    if (g == NULL) return;  // If the NAND gate is NULL, return immediately
    // Reduce output connections for the inputs of the NAND gate
    for (unsigned int i = 0; i < g->number_of_linked_inputs; i++) {
        if (g->linked_inputs[i] != NULL && g->linked_inputs[i]->type == true && g->linked_inputs[i] != g) {
            nand_output_reduction(g, g->linked_inputs[i], false);
        }
    }
    // Delete signal NAND gates connected to the inputs of the NAND gate
    for (unsigned int i = 0; i < g->number_of_linked_inputs; i++) {
        if (g->linked_inputs[i] != NULL && g->linked_inputs[i]->type == false) {
            nand_signal_delete(g->linked_inputs[i]);
        }
    }
    // Reduce input connections for the outputs of the NAND gate
    for (unsigned int i = 0; i < g->size_of_array; i++) nand_input_reduction(g, g->linked_outputs[i]);
    if (g->linked_inputs != NULL) {
        free(g->linked_inputs); 
    }
    if (g->linked_outputs != NULL) {
        free(g->linked_outputs); 
    }
    
    free(g);  // Free memory allocated for the NAND gate
}
int            nand_connect_nand(nand_t * g_out, nand_t * g_in, unsigned k) {
    // Connect a NAND gate to another NAND gate.
    int help = 0;
    if (g_out == NULL || g_in == NULL || k >= g_in->number_of_linked_inputs) {
        errno = EINVAL;
        return -1;
    }

    if (g_in->linked_inputs[k] == NULL) {
        // If the specified input connection is empty, add the output NAND gate to the inputs
        help = nand_output_addition(g_in, g_out);
        if (help == -1) {
            errno = ENOMEM;
            return -1;
        }
        g_in->linked_inputs[k] = g_out;
    } 
    else if (g_in->linked_inputs[k]->type == true) {
        // If the specified input connection is connected to a NAND gate, replace it with the output NAND gate
        help = nand_output_addition(g_in, g_out);
        if (help == -1) {
            errno = ENOMEM; 
            return -1;
        }
        nand_output_reduction(g_in, g_in->linked_inputs[k], true);
        g_in->linked_inputs[k] = g_out;
    } 
    else {
        // If the specified input connection is connected to a signal NAND gate, replace it with the output NAND gate
        help = nand_output_addition(g_in, g_out);
        if (help == -1) {
            errno = ENOMEM;
            return -1;
        }
        nand_signal_delete(g_in->linked_inputs[k]);
        g_in->linked_inputs[k] = g_out;
    }

    return 0;  // Return 0 to indicate success
}
int            nand_connect_signal(bool const * s, nand_t * g_in, unsigned k) {
    // Connect a signal to a NAND gate.
    if (s == NULL || g_in == NULL || g_in->number_of_linked_inputs <= k) {
        // If the signal or the NAND gate is NULL, or k is out of range, set errno to EINVAL and return -1
        errno = EINVAL;
        return -1;
    }
    nand_t *help1 = nand_new_signal(s);
    if (help1 == NULL) {
        // If memory allocation fails for the new signal NAND gate, set errno to ENOMEM and return -1
        errno = ENOMEM;
        free(help1);
        return -1;
    }
    else if(g_in->linked_inputs[k] == NULL) {
        // If the specified input connection is empty, connect the signal NAND gate
        g_in->linked_inputs[k] = help1;
    } 
    else if (g_in->linked_inputs[k]->type == false) {
        // If the specified input connection is connected to a signal, replace it with the new signal
        nand_signal_delete(g_in->linked_inputs[k]);
        g_in->linked_inputs[k] = help1;
    } 
    else {
        // If the specified input connection is connected to a NAND gate, replace it with the new signal
        nand_output_reduction(g_in, g_in->linked_inputs[k], true);
        g_in->linked_inputs[k] = help1;
    }

    return 0;  // Return 0 to indicate success
}
ssize_t        nand_fan_out(nand_t const * g) {
    // Get the number of output connections for a NAND gate.
    if (g == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (ssize_t) g->number_of_linked_outputs;  // Return the number of output connections
}
void           *nand_input(nand_t const * g, unsigned k) {
    // Get the input connection of a NAND gate at index k.
    if (g == NULL || k >= (size_t)(g->number_of_linked_inputs)) {
        // If the NAND gate is NULL or k is out of range, set errno to EINVAL and return NULL
        errno = EINVAL;
        return NULL;
    }
    if (g->linked_inputs[k] == NULL) {
        // If the input connection is empty, set errno to 0 and return NULL
        errno = 0;
        return NULL;
    }
    if (g->linked_inputs[k]->type == true) {
        // If the input connection is connected to a NAND gate, return the pointer to it
        return (void *) g->linked_inputs[k];
    }
    if (g->linked_inputs[k]->type == false) {
        // If the input connection is connected to a signal, return the pointer to its value
        return (void *) g->linked_inputs[k]->pointer;
    }
    return NULL;  // Return NULL if none of the conditions are met
}
nand_t         *nand_output(nand_t const * g, ssize_t k) {
    // Get the output connection of a NAND gate at index k.
    if (k >= g->size_of_array || k < 0 || g == NULL || k >= g->number_of_linked_outputs) {
        errno = EINVAL;
        return NULL;
    } 
    else {
        int j = 0;
        int i = 0; 
        while(true) {
            if(j == k && g->linked_outputs[i] != NULL) {
                return g->linked_outputs[i];
            }
            if(g->linked_outputs[i] != NULL) {
                j++;
            }
            i++;
        }
    }
}
static bool    check_if_combinational_circuit_unit(nand_t * g) {
    // Check if a NAND gate forms a cyclic unit.
    bool result = true;
    for (unsigned int i = 0; i < g->number_of_linked_inputs; i++) {
        // Iterate through the input connections of the NAND gate
        if (g->linked_inputs[i] == NULL) {
            // If any input connection is NULL, return false
            return false;
        }
    }
    if (g->checked == 0) {
        // If the NAND gate is not checked, perform cycle detection recursively
        g->checked = 1;
        for (unsigned int i = 0; i < g->number_of_linked_inputs; i++) {
            if (g->linked_inputs[i] != NULL && g->linked_inputs[i]->type) {
                result = (result && check_if_combinational_circuit_unit(g->linked_inputs[i]));
            }
        }
        g->checked = 2;
        return result;
    } 
    else if (g->checked == 1) {
        // If the NAND gate is in the middle of checking, return false to indicate a cycle
        for (unsigned int i = 0; i < g->number_of_linked_inputs; i++) {
            if (g->linked_inputs[i] != NULL && g->linked_inputs[i]->type) {
                if (g->linked_inputs[i]->checked == 1) return false;
            }
        }
        g->checked = 2;
        return true;
    } 
    else if (g->checked == 2) {
        // If the NAND gate is already checked, return true
        return true;
    }
    return true;
}
static int     check_if_combinational_circuit_clean_up(nand_t * g) {
    // Clean up the cycle detection flags for a NAND gate.
    g->checked = 0;
    for (unsigned int i = 0; i < g->number_of_linked_inputs; i++) {
        // Iterate through the input connections of the NAND gate
        if (g->linked_inputs[i] != NULL && g->linked_inputs[i]->type && g->linked_inputs[i]->checked != 0) 
            check_if_combinational_circuit_clean_up(g->linked_inputs[i]);
    }
    return 0;
}
static bool    check_if_combinational_circuit(nand_t ** g, size_t m) {
    // Check if there are cyclic dependencies among NAND gates.
    bool result = true;
    for (unsigned int i = 0; i < (unsigned int) m; i++) {
        if (!check_if_combinational_circuit_unit(g[i])) {
            result = false;
        }
    }
    // Clean up the cycle detection flags for all NAND gates
    for (unsigned int i = 0; i < (unsigned int) m; i++) {
        if (g[i] != NULL) check_if_combinational_circuit_clean_up(g[i]);
    }
    return result;
}
static int     length_of_critical_path(nand_t * g) {
    // Calculate the length of the critical path for a NAND gate.
    int length = 0;
    if (g->if_estimated_path == true && g->type == true) {
        // If the estimated path length is available and the NAND gate is not a signal gate
        length = g->length_of_critical_path;
        length--;
        if (g->number_of_linked_inputs == 0) {
            length--;  // Decrement length if the NAND gate has no inputs
        }
    } 
    else {
        // Recursively calculate the critical path length
        for (unsigned int i = 0; i < g->number_of_linked_inputs; i++) {
            if (g->linked_inputs[i] != NULL && g->linked_inputs[i]->type == true) {
                length = max(length, length_of_critical_path(g->linked_inputs[i]));
            }
        }
    }
    length++;
    g->length_of_critical_path = length;
    g->if_estimated_path = true;
    return length;
}
bool           value_evaluation(nand_t * g) {
    // Evaluate the value of a NAND gate.
    bool value = false;
    if (g->if_estimated_value == true && g->type == true) {
        // If the estimated value is available and the NAND gate is not a signal gate
        value = g->estimated_value;
    } 
    else if (g->type == false) {
        // If the NAND gate is a signal gate, use the stored signal value
        value = *g->pointer;
    } 
    else if (g->number_of_linked_inputs == 1 && g->linked_inputs[0] != NULL) {
        // If the NAND gate has only one input
        if(value_evaluation(g->linked_inputs[0]) == true) {
            value = false;
        }
        else {
            value = true;
        }
    } 
    else if (g->number_of_linked_inputs == 0) {
        value = false;  // If the NAND gate has no inputs, set value to false
    } 
    else {
        // Evaluate the NAND gate based on its inputs
        for (unsigned int i = 0; i < g->number_of_linked_inputs; i++) {
            if (g->linked_inputs[i] != NULL && !value_evaluation(g->linked_inputs[i])) {
                value = true;
            }
        }
    }
    g->if_estimated_value = true;
    g->estimated_value = value;
    return value; 
}
static int     clean_up(nand_t * g) {
    // Clean up the estimation flags for a NAND gate.
    g->if_estimated_value = false;
    g->if_estimated_path = false;
    for (unsigned int i = 0; i < g->number_of_linked_inputs; i++) {
        // Iterate through the input connections of the NAND gate
        if (g->linked_inputs[i] != NULL && g->linked_inputs[i]->type && g->linked_inputs[i]->if_estimated_value) {
            clean_up(g->linked_inputs[i]);
        }
    }
    return 0;  // Return 0 to indicate success
}
ssize_t        nand_evaluate(nand_t ** g, bool * s, size_t m) {
    // Evaluate the NAND gates given the input signals.
    // Error detection.
    if (g == NULL || s == NULL) {
        errno = EINVAL;
        return -1;
    } 
    else if ((int) m <= 0) {
        errno = EINVAL;
        return -1;
    } 
    for(int i=0; i < (int)m; i++) {
        if(g[i] == NULL) {
            errno = EINVAL;
            return -1;
        }
    }
    if(check_if_combinational_circuit(g,m) == false) {
        errno = ECANCELED;
        return -1;
    } 
    else {
        int result = 0;  // Initialize the result variable to store the length of the critical path
        for (unsigned int i = 0; i < (unsigned int) m; i++) {
            // Iterate through the array of NAND gates
            if (g[i]->type == false) {
                s[i] = *(g[i]->pointer);
            } 
            else if (g[i]->number_of_linked_inputs == 0) {
                // If the NAND gate has no inputs, set the output value to false
                s[i] = false;
            } 
            else {
                // Evaluate the NAND gate and set the output value
                s[i] = value_evaluation(g[i]);
                result = max(result, length_of_critical_path(g[i]));
            }
        }
        // Clean up estimation flags for all NAND gates before returning the length of the critical path
        for (unsigned int i = 0; i < (unsigned int) m; i++) {
            clean_up(g[i]);
        }
        return result;  // Return the length of the critical path
    }
}
/**\file */
#ifndef SLIC_DECLARATIONS_loop_example8SIM_H
#define SLIC_DECLARATIONS_loop_example8SIM_H
#include "MaxSLiCInterface.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Simple static function for the interface 'default'
 * 
 * \param [in] ticks_ColSumKernel Scalar parameter
 * \param [in] instream_input stream input
 * \param [in] instream_size_input size of the stream instream_input in bytes
 * \param [out] outstream_output stream output
 * \param [in] outstream_size_output size of the stream outstream_output in bytes
 */
void loop_example8SIM(
	uint64_t ticks_ColSumKernel,
	const void *instream_input,
	size_t instream_size_input,
	void *outstream_output,
	size_t outstream_size_output);

/**
 * \brief Advanced static interface, structure for the engine interface 'default'
 * 
 */
typedef struct { 
	uint64_t ticks_ColSumKernel; /**<  [in] Scalar parameter */
	const void *instream_input; /**<  [in] stream input */
	size_t instream_size_input; /**<  [in] size of the stream instream_input in bytes */
	void *outstream_output; /**<  [out] stream output */
	size_t outstream_size_output; /**<  [in] size of the stream outstream_output in bytes */
} loop_example8SIM_actions_t;

/**
 * \brief Advanced static function for the interface 'default'
 * \param [in] engine the engine on which the actions will be executed
 * \param [in,out] interface_actions actions to be executed
 */
void loop_example8SIM_run(
	max_engine_t *engine,
	loop_example8SIM_actions_t *interface_actions);

/**
 * \brief Group run advanced static function for the interface 'default'
 * \param group group to use
 * \param interface_actions actions to run
 *
 * Run the actions on the first device available in the group.
 */
void loop_example8SIM_run_group(max_group_t *group, loop_example8SIM_actions_t *interface_actions);

/**
 * \brief Array run advanced static function for the interface 'default'
 * \param engarray the array of devices to use
 * \param interface_actions the array of actions to run
 *
 * Run the array of actions on the array of engines.  The length of interface_actions
 * must match the size of engarray.
 */
void loop_example8SIM_run_array(max_engarray_t *engarray, loop_example8SIM_actions_t *interface_actions[]);

max_actions_t* loop_example8SIM_convert(max_file_t *maxfile, loop_example8SIM_actions_t *interface_actions);

/**
 * \brief Turn debugging on for the functions in the static 
 *          interface of this maxfile.
 * \param debug_directory directory for the generated files
 */
void loop_example8SIM_debug(const char *debug_directory);

/**
 * \brief Initialise a maxfile.
 */
max_file_t* loop_example8SIM_init(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* SLIC_DECLARATIONS_loop_example8SIM_H */


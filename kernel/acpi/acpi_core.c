#include <target/acpi.h>

static struct acpi_namespace_node acpi_gbl_root = {
	ACPI_ROOT_NAME,
	ACPI_DESC_TYPE_NAMED,
	ACPI_TYPE_DEVICE,
	NULL,
	NULL,
	NULL,
	NULL,
};
struct acpi_namespace_node *acpi_gbl_root_node = &acpi_gbl_root;
static acpi_spinlock_t acpi_gbl_reference_lock;

static const char *acpi_gbl_mutex_names[ACPI_NUM_MUTEX] = {
	"ACPI_MTX_Interpreter",
	"ACPI_MTX_Namespace",
	"ACPI_MTX_Tables",
	"ACPI_MTX_Events",
	"ACPI_MTX_Caches",
	"ACPI_MTX_Memory",
	"ACPI_MTX_CommandComplete",
	"ACPI_MTX_CommandReady"
};

const char *acpi_mutex_name(uint32_t mutex_id)
{
	if (mutex_id > ACPI_MAX_MUTEX)
		return "Invalid Mutex ID";
	
	return (acpi_gbl_mutex_names[mutex_id]);
}

static void acpi_reference_update(struct acpi_reference *reference,
				  int action, int *new_count)
{
	acpi_cpuflags_t flags;

	if (!reference)
		return;

	flags = acpi_os_acquire_lock(&acpi_gbl_reference_lock);
	switch (action) {
	case REF_INCREMENT:
		reference->count++;
		break;
	case REF_DECREMENT:
		reference->count--;
		break;
	default:
		return;
	}
	if (new_count)
		*new_count = reference->count;
	acpi_os_release_lock(&acpi_gbl_reference_lock, flags);
}

void acpi_reference_inc(struct acpi_reference *reference)
{
	acpi_reference_update(reference, REF_INCREMENT, NULL);
}

void acpi_reference_dec(struct acpi_reference *reference)
{
	acpi_reference_update(reference, REF_DECREMENT, NULL);
}

int acpi_reference_dec_and_test(struct acpi_reference *reference)
{
	int count;
	acpi_reference_update(reference, REF_DECREMENT, &count);
	return count;
}

void acpi_reference_set(struct acpi_reference *reference, int count)
{
	acpi_cpuflags_t flags;

	if (!reference)
		return;

	flags = acpi_os_acquire_lock(&acpi_gbl_reference_lock);
	reference->count = count;
	acpi_os_release_lock(&acpi_gbl_reference_lock, flags);
}

int acpi_reference_get(struct acpi_reference *reference)
{
	acpi_cpuflags_t flags;
	int count;

	if (!reference)
		return 0;

	flags = acpi_os_acquire_lock(&acpi_gbl_reference_lock);
	count = reference->count;
	acpi_os_release_lock(&acpi_gbl_reference_lock, flags);

	return count;
}

acpi_status_t acpi_initialize_subsystem(void)
{
	acpi_status_t status;
	
	status = acpi_os_create_lock(&acpi_gbl_reference_lock);
	if (ACPI_FAILURE(status))
		return status;

	return AE_OK;
}

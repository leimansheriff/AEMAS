# VIC_BUF_PTR_LOGIC

Explain: We pass a pointer to the buffer (uint8_t *out_frame) because 
creating the array inside the function would place it on the stack. 
Once the function returns, that memory is freed. By passing a pointer 
to a buffer in the caller's scope, the data survives for the USB driver.

# Work

# Dumb gemini

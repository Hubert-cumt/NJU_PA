TMP_DIR := .make_tmp
export TMP_DIR

.PHONY: count clean

count:
	@mkdir -p $(TMP_DIR)
	@touch $(TMP_DIR)/differences.txt
	@bash count.sh > $(TMP_DIR)/current_branch_count.txt
	@git checkout pa0 -q
	@bash count.sh > $(TMP_DIR)/other_branch_count.txt
	@git checkout - -q
	@awk '{print $$NF}' $(TMP_DIR)/current_branch_count.txt > $(TMP_DIR)/output_current.txt
	@awk '{print $$NF}' $(TMP_DIR)/other_branch_count.txt > $(TMP_DIR)/output_other.txt
	@paste -d' ' $(TMP_DIR)/output_current.txt $(TMP_DIR)/output_other.txt | awk '{print $$1 - $$2}' > $(TMP_DIR)/differences.txt
	@echo "The difference in lines' number of *.c : $$(cat $(TMP_DIR)/differences.txt | awk 'NR==1 {print $$1}')"
	@echo "The difference in lines' number of *.h : $$(cat $(TMP_DIR)/differences.txt | awk 'NR==2 {print $$1}')"
	@echo "The difference in total lines' number : $$(cat $(TMP_DIR)/differences.txt | awk 'NR==3 {print $$1}')"
	@rm -rf $(TMP_DIR)
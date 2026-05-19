#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later

TOPDIR="$1"

CONFIG_FILE="$TOPDIR/.config"
CONFIG_OLD="${CONFIG_FILE}.old"

declare -A CONFIG_DEFAULTS
declare -A CONFIG_TYPES         # bool, string, int, choice
declare -A CONFIG_CHOICES       # For choice type.
declare -A CONFIG_PROMPTS

CONFIG_ORDER=()

config_schema_init() {
    CONFIG_DEFAULTS[ARCH]=""
    CONFIG_TYPES[ARCH]="choice"
    CONFIG_CHOICES[ARCH]="x86"
    CONFIG_PROMPTS[ARCH]="Target architecture"

    CONFIG_DEFAULTS[DEBUG_BUILD]="n"
    CONFIG_TYPES[DEBUG_BUILD]="bool"
    CONFIG_PROMPTS[DEBUG_BUILD]="Debug build"

    CONFIG_DEFAULTS[OPT]="2"
    CONFIG_TYPES[OPT]="choice"
    CONFIG_CHOICES[OPT]="0 1 2 3"
    CONFIG_PROMPTS[OPT]="Optimization level"

    CONFIG_DEFAULTS[DEBUG_SYMS]="n"
    CONFIG_TYPES[DEBUG_SYMS]="bool"
    CONFIG_PROMPTS[DEBUG_SYMS]="Include debug symbols"

    CONFIG_DEFAULTS[KERNEL_CMDLINE]=""
    CONFIG_TYPES[KERNEL_CMDLINE]="string"
    CONFIG_PROMPTS[KERNEL_CMDLINE]="Kernel command line"
}

config_load() {
    declare -gA CURRENT_VALUES

    if [[ -f "$CONFIG_FILE" ]]; then
        while IFS='=' read -r key value; do
            # Skip comments and empty lines.
            [[ -z "$key" || "$key" =~ ^[[:space:]]*# ]] && continue
            key=$(echo "$key" | xargs)
            value=$(echo "$value" | xargs)
            CURRENT_VALUES["$key"]="$value"
        done < "$CONFIG_FILE"
    fi

    # Apply defaults for missing values.
    for var in "${!CONFIG_TYPES[@]}"; do
        if [[ -z "${CURRENT_VALUES[$var]}" ]]; then
            CURRENT_VALUES[$var]="${CONFIG_DEFAULTS[$var]}"
        fi
    done
}

validate_input() {
    local var="$1"
    local input="$2"
    local type="${CONFIG_TYPES[$var]}"

    case "$type" in
        bool)
            [[ "$input" =~ ^[yYnN]$ ]] && return 0
            return 1
            ;;
        int)
            [[ "$input" =~ ^[0-9]+$ ]] && return 0
            return 1
            ;;
        choice)
            local choices=(${CONFIG_CHOICES[$var]})
            for choice in "${choices[@]}"; do
                [[ "$input" == "$choice" ]] && return 0
            done
            return 1
            ;;
        string)
            # Any string is valid.
            return 0
            ;;
    esac
    return 1
}

config_prompt() {
    local var="$1"
    local current="${CURRENT_VALUES[$var]}"
    local type="${CONFIG_TYPES[$var]}"
    local prompt="${CONFIG_PROMPTS[$var]}"

    local prompt_str="$prompt"

    case "$type" in
        bool)
            if [[ "$current" == "y" ]]; then
                prompt_str="$prompt_str [Y/n]: "
            else
                prompt_str="$prompt_str [y/N]: "
            fi
            ;;
        choice)
            local choices=(${CONFIG_CHOICES[$var]})
            prompt_str="$prompt_str (${CONFIG_CHOICES[$var]// /|}) [$current]: "
            ;;
        *)
            prompt_str="$prompt_str [$current]: "
            ;;
    esac

    while true; do
        read -p "$prompt_str" input

        if [[ -z "$input" ]]; then
            input="$current"
        fi

        if [[ "$type" == "bool" ]]; then
            input=$(echo "$input" | tr '[:upper:]' '[:lower:]')
            [[ "$input" == "yes" ]] && input="y"
            [[ "$input" == "no" ]] && input="n"
        fi

        if validate_input "$var" "$input"; then
            CURRENT_VALUES[$var]="$input"
            echo ""
            break
        fi

        echo "  Invalid input. Please try again."
        echo ""
    done
}

config_make() {
    echo "==== Torus build configuration ===="
    echo ""
    echo "Press Enter to accept the current value shown in brackets."
    echo "Choices are shown in parentheses."
    echo ""

    for var in "${!CONFIG_TYPES[@]}"; do
        config_prompt "$var"
    done
}

config_save() {
    # Backup first.
    [[ -f "$CONFIG_FILE" ]] && cp "$CONFIG_FILE" "$CONFIG_OLD"

    cat > "$CONFIG_FILE" << EOF
# Automatically generated file; DO NOT EDIT.
# Oxell Torus build configuration.
EOF

    for var in "${!CONFIG_TYPES[@]}"; do
        local value="${CURRENT_VALUES[$var]}"
        if [[ "${CONFIG_TYPES[$var]}" == "bool" ]]; then
            if [[ "$value" == "y" ]]; then
                value="y"
            else
                value="n"
            fi
        fi
        echo "${var} = ${value}" >> "$CONFIG_FILE"
    done

    echo "export ${!CONFIG_TYPES[@]}" >> "$CONFIG_FILE"

    rel_config_file=$(realpath --relative-to="$TOPDIR" "$CONFIG_FILE")
    echo "Configuration saved to $rel_config_file"
}

config_schema_init
config_load

case "${2:-config}" in
    config)
        config_make
        config_save
        ;;
    show)
        echo "==== Current configuration ===="
        for var in "${!CONFIG_TYPES[@]}"; do
            printf "%-22s = %s\n" "$var" "${CURRENT_VALUES[$var]}"
        done
        ;;
    *)
        echo "I do not accept '$2'."
        exit 1
        ;;
esac

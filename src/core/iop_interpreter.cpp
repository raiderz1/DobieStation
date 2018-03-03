#include <cstdio>
#include <cstdlib>
#include "iop_interpreter.hpp"

void IOP_Interpreter::interpret(IOP &cpu, uint32_t instruction)
{
    if (!instruction)
        return;
    int op = instruction >> 26;
    switch (op)
    {
        case 0x00:
            special(cpu, instruction);
            break;
        case 0x01:
            regimm(cpu, instruction);
            break;
        case 0x02:
            j(cpu, instruction);
            break;
        case 0x03:
            jal(cpu, instruction);
            break;
        case 0x04:
            beq(cpu, instruction);
            break;
        case 0x05:
            bne(cpu, instruction);
            break;
        case 0x06:
            blez(cpu, instruction);
            break;
        case 0x07:
            bgtz(cpu, instruction);
            break;
        case 0x08:
            addi(cpu, instruction);
            break;
        case 0x09:
            addiu(cpu, instruction);
            break;
        case 0x0A:
            slti(cpu, instruction);
            break;
        case 0x0B:
            sltiu(cpu, instruction);
            break;
        case 0x0C:
            andi(cpu, instruction);
            break;
        case 0x0D:
            ori(cpu, instruction);
            break;
        case 0x0F:
            lui(cpu, instruction);
            break;
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            cop(cpu, instruction);
            break;
        case 0x20:
            lb(cpu, instruction);
            break;
        case 0x21:
            lh(cpu, instruction);
            break;
        case 0x22:
            lwl(cpu, instruction);
            break;
        case 0x23:
            lw(cpu, instruction);
            break;
        case 0x24:
            lbu(cpu, instruction);
            break;
        case 0x25:
            lhu(cpu, instruction);
            break;
        case 0x26:
            lwr(cpu, instruction);
            break;
        case 0x28:
            sb(cpu, instruction);
            break;
        case 0x29:
            sh(cpu, instruction);
            break;
        case 0x2A:
            swl(cpu, instruction);
            break;
        case 0x2B:
            sw(cpu, instruction);
            break;
        case 0x2E:
            swr(cpu, instruction);
            break;
        default:
            unknown_op("regular", op);
    }
}

void IOP_Interpreter::j(IOP &cpu, uint32_t instruction)
{
    uint32_t addr = (instruction & 0x3FFFFFF) << 2;
    uint32_t PC = cpu.get_PC();
    addr += (PC + 4) & 0xF0000000;
    cpu.jp(addr);
}

void IOP_Interpreter::jal(IOP &cpu, uint32_t instruction)
{
    uint32_t addr = (instruction & 0x3FFFFFF) << 2;
    uint32_t PC = cpu.get_PC();
    addr += (PC + 4) & 0xF0000000;
    cpu.jp(addr);
    cpu.set_gpr(31, PC + 8);
}

void IOP_Interpreter::beq(IOP &cpu, uint32_t instruction)
{
    int offset = (int16_t)(instruction & 0xFFFF);
    offset <<= 2;
    uint32_t reg1 = cpu.get_gpr((instruction >> 21) & 0x1F);
    uint32_t reg2 = cpu.get_gpr((instruction >> 16) & 0x1F);
    cpu.branch(reg1 == reg2, offset);
}

void IOP_Interpreter::bne(IOP &cpu, uint32_t instruction)
{
    int offset = (int16_t)(instruction & 0xFFFF);
    offset <<= 2;
    uint32_t reg1 = cpu.get_gpr((instruction >> 21) & 0x1F);
    uint32_t reg2 = cpu.get_gpr((instruction >> 16) & 0x1F);
    cpu.branch(reg1 != reg2, offset);
}

void IOP_Interpreter::blez(IOP &cpu, uint32_t instruction)
{
    int offset = (int16_t)(instruction & 0xFFFF);
    offset <<= 2;
    int32_t reg = (int32_t)cpu.get_gpr((instruction >> 21) & 0x1F);
    cpu.branch(reg <= 0, offset);
}

void IOP_Interpreter::bgtz(IOP &cpu, uint32_t instruction)
{
    int offset = (int16_t)(instruction & 0xFFFF);
    offset <<= 2;
    int32_t reg = (int32_t)cpu.get_gpr((instruction >> 21) & 0x1F);
    cpu.branch(reg > 0, offset);
}

void IOP_Interpreter::addi(IOP &cpu, uint32_t instruction)
{
    int16_t imm = (int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t source = (instruction >> 21) & 0x1F;
    uint32_t result = cpu.get_gpr(source);
    result += imm;
    cpu.set_gpr(dest, result);
}

void IOP_Interpreter::addiu(IOP &cpu, uint32_t instruction)
{
    int16_t imm = (int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t source = (instruction >> 21) & 0x1F;
    uint32_t result = cpu.get_gpr(source);
    result += imm;
    cpu.set_gpr(dest, result);
}

void IOP_Interpreter::slti(IOP &cpu, uint32_t instruction)
{
    int32_t imm = (int32_t)(int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    int32_t source = (instruction >> 21) & 0x1F;
    source = (int32_t)cpu.get_gpr(source);
    cpu.set_gpr(dest, source < imm);
}

void IOP_Interpreter::sltiu(IOP &cpu, uint32_t instruction)
{
    uint32_t imm = instruction & 0xFFFF;
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t source = (instruction >> 21) & 0x1F;
    source = cpu.get_gpr(source);
    cpu.set_gpr(dest, source < imm);
}

void IOP_Interpreter::andi(IOP &cpu, uint32_t instruction)
{
    uint32_t imm = instruction & 0xFFFF;
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t source = (instruction >> 21) & 0x1F;
    cpu.set_gpr(dest, cpu.get_gpr(source) & imm);
}

void IOP_Interpreter::ori(IOP &cpu, uint32_t instruction)
{
    uint32_t imm = instruction & 0xFFFF;
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t source = (instruction >> 21) & 0x1F;
    cpu.set_gpr(dest, cpu.get_gpr(source) | imm);
}

void IOP_Interpreter::lui(IOP &cpu, uint32_t instruction)
{
    uint32_t imm = (instruction & 0xFFFF) << 16;
    uint32_t dest = (instruction >> 16) & 0x1F;
    cpu.set_gpr(dest, imm);
}

void IOP_Interpreter::lb(IOP &cpu, uint32_t instruction)
{
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;
    cpu.set_gpr(dest, (int32_t)(int8_t)cpu.read8(addr));
}

void IOP_Interpreter::lh(IOP &cpu, uint32_t instruction)
{
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;
    cpu.set_gpr(dest, (int32_t)(int16_t)cpu.read16(addr));
}

void IOP_Interpreter::lwl(IOP &cpu, uint32_t instruction)
{
    static const uint32_t MASK[] = {0xFF000000, 0xFFFF0000, 0xFFFFFF00, 0xFFFFFFFF};
    static const uint32_t SHIFT[] = {24, 16, 8, 0};
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;

    int bits = addr & 0x3;
    uint32_t word = cpu.read32(addr & ~0x3) << SHIFT[bits];
    uint32_t reg = cpu.get_gpr(dest);
    reg &= ~MASK[bits];
    reg |= word & MASK[bits];
    cpu.set_gpr(dest, reg);
}

void IOP_Interpreter::lwr(IOP &cpu, uint32_t instruction)
{
    static const uint32_t MASK[] = {0xFFFFFFFF, 0x00FFFFFF, 0x0000FFFF, 0x000000FF};
    static const uint32_t SHIFT[] = {0, 8, 16, 24};
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;

    int bits = addr & 0x3;
    uint32_t word = cpu.read32(addr & ~0x3) >> SHIFT[bits];
    uint32_t reg = cpu.get_gpr(dest);
    reg &= ~MASK[bits];
    reg |= word & MASK[bits];
    cpu.set_gpr(dest, reg);
}

void IOP_Interpreter::lw(IOP &cpu, uint32_t instruction)
{
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;
    cpu.set_gpr(dest, cpu.read32(addr));
}

void IOP_Interpreter::lbu(IOP &cpu, uint32_t instruction)
{
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;
    cpu.set_gpr(dest, cpu.read8(addr));
}

void IOP_Interpreter::lhu(IOP &cpu, uint32_t instruction)
{
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t dest = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;
    cpu.set_gpr(dest, cpu.read16(addr));
}

void IOP_Interpreter::sb(IOP &cpu, uint32_t instruction)
{
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t source = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;
    cpu.write8(addr, cpu.get_gpr(source) & 0xFF);
}

void IOP_Interpreter::sh(IOP &cpu, uint32_t instruction)
{
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t source = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;
    cpu.write16(addr, cpu.get_gpr(source) & 0xFFFF);
}

void IOP_Interpreter::swl(IOP &cpu, uint32_t instruction)
{
    static const uint32_t MASK[] = {0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF};
    static const uint32_t SHIFT[] = {24, 16, 8, 0};
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t source = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;

    int bits = addr & 0x3;
    uint32_t word = cpu.read32(addr & ~0x3) >> SHIFT[bits];
    uint32_t reg = cpu.get_gpr(source);
    reg &= ~MASK[bits];
    reg |= word & MASK[bits];
    cpu.write32(addr & ~0x3, reg);
}

void IOP_Interpreter::sw(IOP &cpu, uint32_t instruction)
{
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t source = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;
    cpu.write32(addr, cpu.get_gpr(source));
}

void IOP_Interpreter::swr(IOP &cpu, uint32_t instruction)
{
    static const uint32_t MASK[] = {0xFFFFFFFF, 0xFFFFFF00, 0xFFFF0000, 0xFF000000};
    static const uint32_t SHIFT[] = {0, 8, 16, 24};
    int16_t offset = (int16_t)(instruction & 0xFFFF);
    uint32_t source = (instruction >> 16) & 0x1F;
    uint32_t base = (instruction >> 21) & 0x1F;
    uint32_t addr = cpu.get_gpr(base);
    addr += offset;

    int bits = addr & 0x3;
    uint32_t word = cpu.read32(addr & ~0x3) << SHIFT[bits];
    uint32_t reg = cpu.get_gpr(source);
    reg &= ~MASK[bits];
    reg |= word & MASK[bits];
    cpu.write32(addr & ~0x3, reg);
}

void IOP_Interpreter::special(IOP &cpu, uint32_t instruction)
{
    int op = instruction & 0x3F;
    switch (op)
    {
        case 0x00:
            sll(cpu, instruction);
            break;
        case 0x02:
            srl(cpu, instruction);
            break;
        case 0x03:
            sra(cpu, instruction);
            break;
        case 0x04:
            sllv(cpu, instruction);
            break;
        case 0x06:
            srlv(cpu, instruction);
            break;
        case 0x07:
            srav(cpu, instruction);
            break;
        case 0x08:
            jr(cpu, instruction);
            break;
        case 0x09:
            jalr(cpu, instruction);
            break;
        case 0x0C:
            syscall(cpu, instruction);
            break;
        case 0x10:
            mfhi(cpu, instruction);
            break;
        case 0x11:
            mthi(cpu, instruction);
            break;
        case 0x12:
            mflo(cpu, instruction);
            break;
        case 0x13:
            mtlo(cpu, instruction);
            break;
        case 0x19:
            multu(cpu, instruction);
            break;
        case 0x1A:
            div(cpu, instruction);
            break;
        case 0x1B:
            divu(cpu, instruction);
            break;
        case 0x20:
            add(cpu, instruction);
            break;
        case 0x21:
            addu(cpu, instruction);
            break;
        case 0x23:
            subu(cpu, instruction);
            break;
        case 0x24:
            and_cpu(cpu, instruction);
            break;
        case 0x25:
            or_cpu(cpu, instruction);
            break;
        case 0x26:
            xor_cpu(cpu, instruction);
            break;
        case 0x27:
            nor(cpu, instruction);
            break;
        case 0x2A:
            slt(cpu, instruction);
            break;
        case 0x2B:
            sltu(cpu, instruction);
            break;
        default:
            unknown_op("special", op);
    }
}

void IOP_Interpreter::sll(IOP &cpu, uint32_t instruction)
{
    uint32_t source = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    uint32_t shift = (instruction >> 6) & 0x1F;
    source = cpu.get_gpr(source);
    source <<= shift;
    cpu.set_gpr(dest, source);
}

void IOP_Interpreter::srl(IOP &cpu, uint32_t instruction)
{
    uint32_t source = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    uint32_t shift = (instruction >> 6) & 0x1F;
    source = cpu.get_gpr(source);
    source >>= shift;
    cpu.set_gpr(dest, source);
}

void IOP_Interpreter::sra(IOP &cpu, uint32_t instruction)
{
    int32_t source = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    uint32_t shift = (instruction >> 6) & 0x1F;
    source = (int32_t)cpu.get_gpr(source);
    source >>= shift;
    cpu.set_gpr(dest, (uint32_t)source);
}

void IOP_Interpreter::sllv(IOP &cpu, uint32_t instruction)
{
    uint32_t source = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    uint32_t shift = (instruction >> 21) & 0x1F;
    source = cpu.get_gpr(source);
    source <<= cpu.get_gpr(shift) & 0x1F;
    cpu.set_gpr(dest, source);
}

void IOP_Interpreter::srlv(IOP &cpu, uint32_t instruction)
{
    uint32_t source = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    uint32_t shift = (instruction >> 21) & 0x1F;
    source = cpu.get_gpr(source);
    source >>= cpu.get_gpr(shift) & 0x1F;
    cpu.set_gpr(dest, source);
}

void IOP_Interpreter::srav(IOP &cpu, uint32_t instruction)
{
    int32_t source = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    uint32_t shift = (instruction >> 21) & 0x1F;
    source = (int32_t)cpu.get_gpr(source);
    source >>= cpu.get_gpr(shift) & 0x1F;
    cpu.set_gpr(dest, (uint32_t)source);
}

void IOP_Interpreter::jr(IOP &cpu, uint32_t instruction)
{
    uint32_t address = (instruction >> 21) & 0x1F;
    cpu.jp(cpu.get_gpr(address));
}

void IOP_Interpreter::jalr(IOP &cpu, uint32_t instruction)
{
    uint32_t new_addr = (instruction >> 21) & 0x1F;
    uint32_t return_reg = (instruction >> 11) & 0x1F;
    uint32_t return_addr = cpu.get_PC() + 8;
    cpu.jp(cpu.get_gpr(new_addr));
    cpu.set_gpr(return_reg, return_addr);
}

void IOP_Interpreter::syscall(IOP &cpu, uint32_t instruction)
{
    cpu.syscall_exception();
}

void IOP_Interpreter::mfhi(IOP &cpu, uint32_t instruction)
{
    uint32_t dest = (instruction >> 11) & 0x1F;
    cpu.set_gpr(dest, cpu.get_HI());
}

void IOP_Interpreter::mthi(IOP &cpu, uint32_t instruction)
{
    uint32_t source = (instruction >> 21) & 0x1F;
    cpu.set_HI(cpu.get_gpr(source));
}

void IOP_Interpreter::mflo(IOP &cpu, uint32_t instruction)
{
    uint32_t dest = (instruction >> 11) & 0x1F;
    cpu.set_gpr(dest, cpu.get_LO());
}

void IOP_Interpreter::mtlo(IOP &cpu, uint32_t instruction)
{
    uint32_t source = (instruction >> 21) & 0x1F;
    cpu.set_LO(cpu.get_gpr(source));
}

void IOP_Interpreter::multu(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    uint64_t temp = op1 * op2;
    cpu.set_LO(temp & 0xFFFFFFFF);
    cpu.set_HI(temp >> 32);
}

void IOP_Interpreter::div(IOP &cpu, uint32_t instruction)
{
    int32_t op1 = (instruction >> 21) & 0x1F;
    int32_t op2 = (instruction >> 16) & 0x1F;
    op1 = (int32_t)cpu.get_gpr(op1);
    op2 = (int32_t)cpu.get_gpr(op2);
    if (!op2)
    {
        printf("\n[IOP_Interpreter] DIV division by zero\n");
        exit(1);
    }
    cpu.set_LO(op1 / op2);
    cpu.set_HI(op1 % op2);
}

void IOP_Interpreter::divu(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    if (!op2)
    {
        printf("\n[IOP_Interpreter] DIV division by zero\n");
        exit(1);
    }
    cpu.set_LO(op1 / op2);
    cpu.set_HI(op1 % op2);
}

void IOP_Interpreter::add(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    cpu.set_gpr(dest, op1 + op2);
}

void IOP_Interpreter::addu(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    cpu.set_gpr(dest, op1 + op2);
}

void IOP_Interpreter::subu(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    cpu.set_gpr(dest, op1 - op2);
}

void IOP_Interpreter::and_cpu(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    cpu.set_gpr(dest, op1 & op2);
}

void IOP_Interpreter::or_cpu(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    cpu.set_gpr(dest, op1 | op2);
}

void IOP_Interpreter::xor_cpu(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    cpu.set_gpr(dest, op1 ^ op2);
}

void IOP_Interpreter::nor(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    cpu.set_gpr(dest, ~(op1 | op2));
}

void IOP_Interpreter::slt(IOP &cpu, uint32_t instruction)
{
    int32_t op1 = (instruction >> 21) & 0x1F;
    int32_t op2 = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    op1 = (int32_t)cpu.get_gpr(op1);
    op2 = (int32_t)cpu.get_gpr(op2);
    cpu.set_gpr(dest, op1 < op2);
}

void IOP_Interpreter::sltu(IOP &cpu, uint32_t instruction)
{
    uint32_t op1 = (instruction >> 21) & 0x1F;
    uint32_t op2 = (instruction >> 16) & 0x1F;
    uint32_t dest = (instruction >> 11) & 0x1F;
    op1 = cpu.get_gpr(op1);
    op2 = cpu.get_gpr(op2);
    cpu.set_gpr(dest, op1 < op2);
}

void IOP_Interpreter::regimm(IOP &cpu, uint32_t instruction)
{
    int op = (instruction >> 16) & 0x1F;
    switch (op)
    {
        case 0x00:
            bltz(cpu, instruction);
            break;
        case 0x01:
            bgez(cpu, instruction);
            break;
        default:
            unknown_op("regimm", op);
    }
}

void IOP_Interpreter::bltz(IOP &cpu, uint32_t instruction)
{
    int offset = (int16_t)(instruction & 0xFFFF);
    offset <<= 2;
    int32_t reg = (int32_t)cpu.get_gpr((instruction >> 21) & 0x1F);
    cpu.branch(reg < 0, offset);
}

void IOP_Interpreter::bgez(IOP &cpu, uint32_t instruction)
{
    int offset = (int16_t)(instruction & 0xFFFF);
    offset <<= 2;
    int32_t reg = (int32_t)cpu.get_gpr((instruction >> 21) & 0x1F);
    cpu.branch(reg >= 0, offset);
}

void IOP_Interpreter::cop(IOP &cpu, uint32_t instruction)
{
    int op = (instruction >> 21) & 0x1F;
    uint8_t cop_id = ((instruction >> 26) & 0x3);
    op |= cop_id << 8;
    switch (op)
    {
        case 0x000:
            mfc(cpu, instruction);
            break;
        case 0x004:
            mtc(cpu, instruction);
            break;
        case 0x010:
            cpu.rfe();
            break;
        default:
            unknown_op("cop", op);
    }
}

void IOP_Interpreter::mfc(IOP &cpu, uint32_t instruction)
{
    uint8_t cop_id = (instruction >> 26) & 0x3;
    uint8_t reg = (instruction >> 16) & 0x1F;
    uint8_t cop_reg = (instruction >> 11) & 0x1F;
    cpu.mfc(cop_id, cop_reg, reg);
}

void IOP_Interpreter::mtc(IOP &cpu, uint32_t instruction)
{
    uint8_t cop_id = (instruction >> 26) & 0x3;
    uint8_t reg = (instruction >> 16) & 0x1F;
    uint8_t cop_reg = (instruction >> 11) & 0x1F;
    cpu.mtc(cop_id, cop_reg, reg);
}

void IOP_Interpreter::unknown_op(const char *type, uint16_t op)
{
    printf("\n[IOP_Interpreter] Unrecognized %s op $%02X\n", type, op);
    exit(1);
}
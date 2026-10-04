#pragma once

#include "application/ports/books.hpp"
#include "application/ports/clock.hpp"

#include <algorithm>
#include <map>

namespace bookshelf::tests
{

// Une date fixe : le test donne le même résultat aujourd'hui et dans dix ans.
class FixedClock : public application::IClock
{
public:
    explicit FixedClock(std::chrono::year_month_day day = std::chrono::year{2026} / 10 / 4)
        : day_(day)
    {
    }

    [[nodiscard]] std::chrono::year_month_day today() const override { return day_; }

private:
    std::chrono::year_month_day day_;
};

// Des livres rangés dans une std::map, en mémoire. Aucune base, aucun fichier.
class FakeBooks : public application::IBooks
{
public:
    domain::Id<domain::Book> create(const domain::Book& book) override
    {
        domain::Book copy = book;
        copy.id = {nextId_++};
        books_[copy.id.value] = copy;
        return copy.id;
    }

    std::optional<domain::Book> get(domain::Id<domain::Book> id) override
    {
        const auto found = books_.find(id.value);
        if (found == books_.end())
        {
            return std::nullopt;
        }
        return found->second;
    }

    bool update(const domain::Book& book) override
    {
        const auto found = books_.find(book.id.value);
        if (found == books_.end())
        {
            return false;
        }
        found->second = book;
        return true;
    }

    bool remove(domain::Id<domain::Book> id) override { return books_.erase(id.value) > 0; }

    std::vector<domain::Book> list(std::string_view search) override
    {
        std::vector<domain::Book> result;
        for (const auto& [id, book] : books_)
        {
            if (search.empty() || book.title.contains(search) || book.author.contains(search))
            {
                result.push_back(book);
            }
        }
        std::ranges::sort(result, {}, &domain::Book::title);
        return result;
    }

private:
    std::map<std::int64_t, domain::Book> books_;
    std::int64_t nextId_ = 1;
};

} // namespace bookshelf::tests
